{
    This file is part of the Free Pascal run time library.
    Copyright (c) 2000 by Florian Klaempfl
    member of the Free Pascal development team.

    This file implements the helper routines for dyn. Arrays in FPC

    See the file COPYING.FPC, included in this distribution,
    for details about the copyright.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 **********************************************************************
}

type
   { don't add new fields, the size is used }
   { to calculate memory requirements       }
   pdynarray = ^tdynarray;
   tdynarray = packed record
      refcount : ptrint;
      high : tdynarrayindex;
   end;

   pdynarraytypedata = ^tdynarraytypedata;
   tdynarraytypedata =
{$ifndef FPC_REQUIRES_PROPER_ALIGNMENT}
   packed
{$else}
  {$ifdef powerpc64}
    { 3.0.0 does not align elType field on a 8-byte boundary,
      thus use packed also in this case }
    {$ifdef VER3_0_0}
      packed
    {$endif VER3_0_0}
  {$endif powerpc64}

{$endif FPC_REQUIRES_PROPER_ALIGNMENT}
   record
     elSize : SizeUInt;
     {$ifdef VER3_0}
     elType2 : Pointer;
     {$else}
     elType2 : PPointer;
     {$endif}
     varType : Longint;
     {$ifdef VER3_0}
     elType : Pointer;
     {$else}
     elType : PPointer;
     {$endif}
   end;

procedure fpc_dynarray_rangecheck(p : pointer;i : tdynarrayindex);[Public,Alias:'FPC_DYNARRAY_RANGECHECK']; compilerproc;
  begin
     if not(assigned(p)) or (i<0) or (i>pdynarray(p-sizeof(tdynarray))^.high) then
       HandleErrorAddrFrameInd(201,get_pc_addr,get_frame);
  end;


function fpc_dynarray_length(p : pointer) : tdynarrayindex;[Public,Alias:'FPC_DYNARRAY_LENGTH']; compilerproc;
  begin
     if assigned(p) then
       fpc_dynarray_length:=pdynarray(p-sizeof(tdynarray))^.high+1
     else
       fpc_dynarray_length:=0;
  end;


function fpc_dynarray_high(p : pointer) : tdynarrayindex;[Public,Alias:'FPC_DYNARRAY_HIGH']; compilerproc;
  begin
     if assigned(p) then
       fpc_dynarray_high:=pdynarray(p-sizeof(tdynarray))^.high
     else
       fpc_dynarray_high:=-1;
  end;


procedure fpc_dynarray_clear(var p : pointer;ti : pointer); [Public,Alias:'FPC_DYNARRAY_CLEAR']; compilerproc;
  var
     realp : pdynarray;
  begin
    if (P=Nil) then
      exit;
    realp:=pdynarray(p-sizeof(tdynarray));
    if realp^.refcount=0 then
      HandleErrorAddrFrameInd(204,get_pc_addr,get_frame);

    if declocked(realp^.refcount) then
      begin
{$ifdef VER3_0}
        ti:=aligntoptr(ti+2+PByte(ti)[1]);
{$else VER3_0}
        ti:=aligntoqword(ti+2+PByte(ti)[1]);
{$endif VER3_0}
        if assigned(pdynarraytypedata(ti)^.elType) then
          int_finalizearray(p,pdynarraytypedata(ti)^.elType{$ifndef VER3_0}^{$endif},realp^.high+1);
        freemem(realp);
      end;
    p:=nil;
  end;

{ alias for internal use }
Procedure fpc_dynarray_clear (var p : pointer;ti : pointer);[external name 'FPC_DYNARRAY_CLEAR'];


procedure fpc_dynarray_incr_ref(p : pointer);[Public,Alias:'FPC_DYNARRAY_INCR_REF']; compilerproc;
  var
     realp : pdynarray;
  begin
     if p=nil then
       exit;

     realp:=pdynarray(p-sizeof(tdynarray));
     if realp^.refcount=0 then
       HandleErrorAddrFrameInd(204,get_pc_addr,get_frame);

     inclocked(realp^.refcount);
  end;

{ provide local access to dynarr_decr_ref for dynarr_setlength }
procedure fpc_dynarray_incr_ref(p : pointer); [external name 'FPC_DYNARRAY_INCR_REF'];


procedure fpc_dynarray_assign(var dest: Pointer; src: Pointer; ti: pointer);[public,alias:'FPC_DYNARRAY_ASSIGN']; compilerproc;
  begin
    fpc_dynarray_incr_ref(src);
    fpc_dynarray_clear(dest,ti);
    Dest:=Src;
  end;

procedure fpc_dynarray_assign(var dest: Pointer; src: Pointer; ti: pointer);[external name 'FPC_DYNARRAY_ASSIGN'];

{ provide local access to dynarr_setlength }
procedure int_dynarray_setlength(var p : pointer;pti : pointer;
  dimcount : sizeint;dims : pdynarrayindex);[external name 'FPC_DYNARR_SETLENGTH'];

procedure fpc_dynarray_setlength(var p : pointer;pti : pointer;
  dimcount : sizeint;dims : pdynarrayindex);[Public,Alias:'FPC_DYNARR_SETLENGTH']; compilerproc;

  var
     i : tdynarrayindex;
     movelen,
     size : sizeint;
     { contains the "fixed" pointers where the refcount }
     { and high are at positive offsets                 }
     realp,newp : pdynarray;
     ti : pointer;
     updatep: boolean;
     elesize : sizeint;
     eletype,eletypemngd : pointer;
     movsize : sizeint;

  begin
     { negative length is not allowed }
     if dims[0]<0 then
       HandleErrorAddrFrameInd(201,get_pc_addr,get_frame);

     { skip kind and name }
{$ifdef VER3_0}
     ti:=aligntoptr(Pointer(pti)+2+PByte(pti)[1]);
{$else VER3_0}
     ti:=aligntoqword(Pointer(pti)+2+PByte(pti)[1]);
{$endif VER3_0}

     elesize:=pdynarraytypedata(ti)^.elSize;
     {$ifdef VER3_0}
     eletype:=pdynarraytypedata(ti)^.elType2;
     {$else}
     eletype:=pdynarraytypedata(ti)^.elType2^;
     {$endif}
     { only set if type needs finalization }
     {$ifdef VER3_0}
     eletypemngd:=pdynarraytypedata(ti)^.elType;
     {$else}
     if assigned(pdynarraytypedata(ti)^.elType) then
       eletypemngd:=pdynarraytypedata(ti)^.elType^
     else
       eletypemngd:=nil;
     {$endif}

     { determine new memory size }
     size:=elesize*dims[0]+sizeof(tdynarray);
     updatep := false;

     { not assigned yet? }
     if not(assigned(p)) then
       begin
          { do we have to allocate memory? }
          if dims[0] = 0 then
            exit;
          getmem(newp,size);
          fillchar(newp^,size,0);
{$ifdef FPC_HAS_MANAGEMENT_OPERATORS}
          { call int_InitializeArray for management operators }
          if assigned(eletypemngd) and (PByte(eletype)^ in [tkRecord, tkObject]) then
            int_InitializeArray(pointer(newp)+sizeof(tdynarray), eletype, dims[0]);
{$endif FPC_HAS_MANAGEMENT_OPERATORS}
          updatep := true;
       end
     else
       begin
          { if the new dimension is 0, we've to release all data }
          if dims[0]=0 then
            begin
               fpc_dynarray_clear(p,pti);
               exit;
            end;

          realp:=pdynarray(p-sizeof(tdynarray));
          newp := realp;

          if realp^.refcount<>1 then
            begin
               updatep := true;
               { make an unique copy }
               getmem(newp,size);
               fillchar(newp^,sizeof(tdynarray),0);
               if realp^.high < dims[0] then
                 movelen := realp^.high+1
               else
                 movelen := dims[0];
               movsize := elesize*movelen;
               move(p^,(pointer(newp)+sizeof(tdynarray))^, movsize);
               if size-sizeof(tdynarray)>movsize then
                 fillchar((pointer(newp)+sizeof(tdynarray)+movsize)^,size-sizeof(tdynarray)-movsize,0);

               { increment ref. count of managed members }
               if assigned(eletypemngd) then
                 for i:= 0 to movelen-1 do
                   int_addref(pointer(newp)+sizeof(tdynarray)+elesize*i,eletypemngd);

               { a declock(ref. count) isn't enough here }
               { it could be that the in MT environments  }
               { in the mean time the refcount was       }
               { decremented                             }

               { it is, because it doesn't really matter }
               { if the array is now removed             }
               fpc_dynarray_clear(p,pti);
            end
          else if dims[0]<>realp^.high+1 then
            begin
               { range checking is quite difficult ...  }
               { if size overflows then it is less than }
               { the values it was calculated from      }
               if (size<sizeof(tdynarray)) or
                 ((elesize>0) and (size<elesize)) then
                 HandleErrorAddrFrameInd(201,get_pc_addr,get_frame);

               { resize? }
               { here, realp^.refcount has to be one, otherwise the previous }
               { if-statement would have been taken. Or is this also for MT  }
               { code? (JM)                                                  }
               if realp^.refcount=1 then
                 begin
                    { shrink the array? }
                    if dims[0]<realp^.high+1 then
                      begin
                         if assigned(eletypemngd) then
                           int_finalizearray(pointer(realp)+sizeof(tdynarray)+
                              elesize*dims[0],
                              eletypemngd,realp^.high-dims[0]+1);
                         reallocmem(realp,size);
                      end
                    else if dims[0]>realp^.high+1 then
                      begin
                         reallocmem(realp,size);
                         fillchar((pointer(realp)+sizeof(tdynarray)+elesize*(realp^.high+1))^,
                           (dims[0]-realp^.high-1)*elesize,0);
{$ifdef FPC_HAS_MANAGEMENT_OPERATORS}
                         { call int_InitializeArray for management operators }
                         if assigned(eletypemngd) and (PByte(eletype)^ in [tkRecord, tkObject]) then
                           int_InitializeArray(pointer(realp)+sizeof(tdynarray)+elesize*(realp^.high+1),
                             eletype, dims[0]-realp^.high-1);
{$endif FPC_HAS_MANAGEMENT_OPERATORS}
                      end;
                    newp := realp;
                    updatep := true;
                 end;
            end;
       end;
    { handle nested arrays }
    if dimcount>1 then
      begin
         for i:=0 to dims[0]-1 do
           int_dynarray_setlength(pointer((pointer(newp)+sizeof(tdynarray)+i*elesize)^),
             eletype,dimcount-1,@dims[1]);
      end;
     if updatep then
       begin
         p:=pointer(newp)+sizeof(tdynarray);
         newp^.refcount:=1;
         newp^.high:=dims[0]-1;
       end;
  end;


{ provide local access to dynarr_copy }
function int_dynarray_copy(psrc : pointer;ti : pointer;
    lowidx,count:tdynarrayindex) : fpc_stub_dynarray;[external name 'FPC_DYNARR_COPY'];

function fpc_dynarray_copy(psrc : pointer;ti : pointer;
    lowidx,count:tdynarrayindex) : fpc_stub_dynarray;[Public,Alias:'FPC_DYNARR_COPY'];compilerproc;
  var
    realpsrc : pdynarray;
    i,size : sizeint;
    elesize : sizeint;
    eletype : pointer;
  begin
     fpc_dynarray_clear(pointer(result),ti);
     if psrc=nil then
       exit;
{$ifndef FPC_DYNARRAYCOPY_FIXED}
     if (lowidx=-1) and (count=-1) then
       begin
         lowidx:=0;
         count:=high(tdynarrayindex);
       end;
{$endif FPC_DYNARRAYCOPY_FIXED}
     realpsrc:=pdynarray(psrc-sizeof(tdynarray));
     if (lowidx<0) then
       begin
       { Decrease count if index is negative, this is different from how copy()
         works on strings. Checked against D7. }
         if count<=0 then
           exit;              { may overflow when adding lowidx }
         count:=count+lowidx;
         lowidx:=0;
       end;
     if (count>realpsrc^.high-lowidx+1) then
       count:=realpsrc^.high-lowidx+1;
     if count<=0 then
       exit;

     { skip kind and name }
{$ifdef VER3_0}
     ti:=aligntoptr(ti+2+PByte(ti)[1]);
{$else VER3_0}
     ti:=aligntoqword(ti+2+PByte(ti)[1]);
{$endif VER3_0}

     elesize:=pdynarraytypedata(ti)^.elSize;
     { only set if type needs finalization }
     {$ifdef VER3_0}
     eletype:=pdynarraytypedata(ti)^.elType;
     {$else}
     if assigned(pdynarraytypedata(ti)^.elType) then
       eletype:=pdynarraytypedata(ti)^.elType^
     else
       eletype:=nil;
     {$endif}

     { create new array }
     size:=elesize*count;
     getmem(pointer(result),size+sizeof(tdynarray));
     pdynarray(result)^.refcount:=1;
     pdynarray(result)^.high:=count-1;
     inc(pointer(result),sizeof(tdynarray));
     { copy data }
     move(pointer(psrc+elesize*lowidx)^,pointer(result)^,size);

     { increment ref. count of members? }
     if assigned(eletype) then
       for i:=0 to count-1 do
         int_addref(pointer(pointer(result)+elesize*i),eletype);
  end;


{$ifndef VER3_0}
procedure fpc_dynarray_delete(var p : pointer;source,count : SizeInt;pti : pointer);
   var
      newhigh,
      i : tdynarrayindex;
      size : sizeint;
      { contains the "fixed" pointers where the refcount }
      { and high are at positive offsets                 }
      realp,newp : pdynarray;
      ti : pointer;
      elesize : sizeint;
      eletype,eletypemngd : pointer;

   begin
     { if source > high then nothing to do }
     if not assigned(p) or
         (source>pdynarray(p-sizeof(tdynarray))^.high) or
         (count<=0) or
         (source<0) then
       exit;
     { cap count }
     if source+count-1>pdynarray(p-sizeof(tdynarray))^.high then
       count:=pdynarray(p-sizeof(tdynarray))^.high-source+1;

     { fast path: delete whole array }
     if (source=0) and (count=pdynarray(p-sizeof(tdynarray))^.high+1) then
       begin
         fpc_dynarray_clear(p,pti);
         exit;
       end;

     { skip kind and name }
{$ifdef VER3_0}
     ti:=aligntoptr(Pointer(pti)+2+PByte(pti)[1]);
{$else VER3_0}
     ti:=aligntoqword(Pointer(pti)+2+PByte(pti)[1]);
{$endif VER3_0}

     elesize:=pdynarraytypedata(ti)^.elSize;
     eletype:=pdynarraytypedata(ti)^.elType2^;
     { only set if type needs finalization }
     if assigned(pdynarraytypedata(ti)^.elType) then
       eletypemngd:=pdynarraytypedata(ti)^.elType^
     else
       eletypemngd:=nil;

     realp:=pdynarray(p-sizeof(tdynarray));
     newp:=realp;

     { determine new memory size }
     newhigh:=realp^.high-count;
     size:=elesize*(newhigh+1)+sizeof(tdynarray);

     if realp^.refcount<>1 then
       begin
          { make an unique copy }
          getmem(newp,size);
          fillchar(newp^,sizeof(tdynarray),0);
          { copy the elements that we still need }
          if source>0 then
            move(p^,(pointer(newp)+sizeof(tdynarray))^,source*elesize);
          if source+count-1<realp^.high then
            move((p+(source+count)*elesize)^,(pointer(newp)+sizeof(tdynarray)+source*elesize)^,(realp^.high-(source+count)+1)*elesize);

          { increment ref. count of managed members }
          if assigned(eletypemngd) then
            for i:=0 to newhigh do
              int_addref(pointer(newp)+sizeof(tdynarray)+elesize*i,eletypemngd);

          { a declock(ref. count) isn't enough here }
          { it could be that the in MT environments  }
          { in the mean time the refcount was       }
          { decremented                             }

          { it is, because it doesn't really matter }
          { if the array is now removed             }
          fpc_dynarray_clear(p,pti);
        end
      else
        begin
          { finalize the elements that will be removed }
          if assigned(eletypemngd) then
            begin
              for i:=source to source+count-1 do
                int_finalize(p+i*elesize,eletype);
            end;

          { close the gap by moving the trailing elements to the front }
          move((p+(source+count)*elesize)^,(p+source*elesize)^,(realp^.high-(source+count)+1)*elesize);

          { resize the array }
          reallocmem(realp,size);
          newp:=realp;
        end;
    p:=pointer(newp)+sizeof(tdynarray);
    newp^.refcount:=1;
    newp^.high:=newhigh;
  end;
{$endif VER3_0}


procedure DynArraySetLength(var a: Pointer; typeInfo: Pointer; dimCnt: SizeInt; lengthVec: PSizeInt);
  external name 'FPC_DYNARR_SETLENGTH';

function DynArraySize(a : pointer): tdynarrayindex;
  external name 'FPC_DYNARRAY_LENGTH';

procedure DynArrayClear(var a: Pointer; typeInfo: Pointer);
  external name 'FPC_DYNARRAY_CLEAR';

function DynArrayDim(typeInfo: Pointer): Integer;
  begin
    result:=0;
    while (typeInfo <> nil) and (pdynarraytypeinfo(typeInfo)^.kind = tkDynArray) do
    begin
      { skip kind and name }
{$ifdef VER3_0}
      typeInfo:=aligntoptr(typeInfo+2+PByte(typeInfo)[1]);
{$else VER3_0}
      typeInfo:=aligntoqword(typeInfo+2+PByte(typeInfo)[1]);
{$endif VER3_0}

      { element type info}
      {$ifdef VER3_0}
      typeInfo:=pdynarraytypedata(typeInfo)^.elType2;
      {$else VER3_0}
      typeInfo:=pdynarraytypedata(typeInfo)^.elType2^;
      {$endif VER3_0}

      Inc(result);
    end;
  end;

function DynArrayBounds(a: Pointer; typeInfo: Pointer): TBoundArray;
  var
    i,dim: sizeint;
  begin
    dim:=DynArrayDim(typeInfo);
    SetLength(result, dim);

    for i:=0 to pred(dim) do
      if a = nil then
        exit
      else
      begin
        result[i]:=DynArraySize(a)-1;
        a:=PPointerArray(a)^[0];
      end;
  end;

function IsDynArrayRectangular(a: Pointer; typeInfo: Pointer): Boolean;
  var
    i,j: sizeint;
    dim,count: sizeint;
  begin
    dim:=DynArrayDim(typeInfo);
    for i:=1 to pred(dim) do
    begin
      count:=DynArraySize(PPointerArray(a)^[0]);

      for j:=1 to Pred(DynArraySize(a)) do
        if count<>DynArraySize(PPointerArray(a)^[j]) then
          exit(false);

      a:=PPointerArray(a)^[0];
    end;
    result:=true;
  end;

function DynArrayIndex(a: Pointer; const indices: array of SizeInt; typeInfo: Pointer): Pointer;
  var
    i,h: sizeint;
  begin
    h:=High(indices);
    for i:=0 to h do
    begin
      if i<h then
        a := PPointerArray(a)^[indices[i]];

      { skip kind and name }
      typeInfo:=(typeInfo+2+PByte(typeInfo)[1]);
      { element type info}
      {$ifdef VER3_0}
      typeInfo:=pdynarraytypedata(typeInfo)^.elType2;
      {$else VER3_0}
      typeInfo:=pdynarraytypedata(typeInfo)^.elType2^;
      {$endif VER3_0}

      if typeInfo=nil then
        exit(nil);
    end;

    { skip kind and name }
    typeInfo:=(typeInfo+2+PByte(typeInfo)[1]);

    result:=@(PByte(a)[indices[h]*pdynarraytypedata(typeInfo)^.elSize]);
  end;

{ obsolete but needed for bootstrapping }
procedure fpc_dynarray_decr_ref(var p : pointer;ti : pointer); [Public,Alias:'FPC_DYNARRAY_DECR_REF']; compilerproc;
  begin
    fpc_dynarray_clear(p,ti);
  end;

