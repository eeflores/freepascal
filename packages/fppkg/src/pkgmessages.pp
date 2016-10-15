unit pkgmessages;

{$mode objfpc}{$H+}

interface


Resourcestring
  SError                     = 'Error: ';
  SWarning                   = 'Warning: ';
  SDebug                     = 'Debug: ';

  SErrInValidArgument        = 'Invalid command-line argument at position %d : %s';
  SErrNeedArgument           = 'Option at position %d (%s) needs an argument';
  SErrMissingFPC             = 'Could not find a fpc executable in the PATH';
  SErrInvalidFPCInfo         = 'Compiler returns invalid information, check if fpc -iV works';
  SErrMissingFPMake          = 'Missing configuration fpmake.pp';
  SErrMissingMakefilefpc     = 'Missing configuration Makefile.fpc';
  SErrMissingDirectory       = 'Missing directory "%s"';
  SErrMissingCompilerConfig  = 'Could not find compiler configuration "%s"';
  SErrMissingInstallPackage  = 'Package "%s" is not installed';
  SErrMissingAvailablePackage= 'Package "%s" is not available';
  SErrMissingPackage         = 'Could not fin package "%s"';
  SErrMissingInstallRepo     = 'Could not find repository "%s"';
  SErrNoPackageSpecified     = 'No package specified';
  SErrNoPackageAvailable     = 'Package %s %s is not available';
  SErrOnlyLocalDir           = 'The specified command "%s" works only on current dir, not on a (remote) package';
  SErrIllConfRepository      = 'Invalid configured repository "%s"';
  SErrExecutionFPMake        = 'Execution of FPMake %s failed';
  SErrException              = 'The FPC Package tool encountered the following error:';
  SErrActionAlreadyRegistered= 'Action "%s" is already registered';
  SErrActionNotFound         = 'Action "%s" is not supported';
  SErrCompileFailureFPMake   = 'Could not compile fpmake driver program';
  SErrCompileFailureFPMakeTryRecovery = 'Could not compile fpmake driver program, try adding "--recovery"';
  SErrNoFTPDownload          = 'This binary has no support for FTP downloads.';
  SErrNoHTTPDownload         = 'This binary has no support for HTTP downloads.';
  SErrBackupFailed           = 'Backup of file "%s" to file "%s" failed.';
  SErrUnknownProtocol        = 'Unknown download protocol "%s" in url "%s"';
  SErrNoSuchFile             = 'File "%s" does not exist.';
  SErrDownloadFailed         = '%s Download of "%s" failed: %s';
  SErrInvalidLogLevels       = 'Invalid verbosity string: "%s"';
  SErrInvalidCommand         = 'Invalid command: %s';
  SErrChangeDirFailed        = 'Could not change directory to "%s"';
  SErrCorruptPackagesFile    = 'Packages file "%s" is corrupt, delete file manual and retry';
  SErrCorruptMirrorsFile     = 'Mirrors file "%s" is corrupt, delete file manual and retry';
  SErrPackageIsLocal         = 'Operation not supported for local packages';
  SErrConvertFPMakeExists    = 'Found existing fpmake.pp, aborting conversion';
  SErrFailedToSelectMirror   = 'Could not select a mirror, run update and retry';
  SErrUnsupportedConfigVersion = 'Configuration file is too old, delete file manual and retry';
  SErrPackageDoesNotSupportTarget = 'Package %s does not support %s';
  SErrHTTPGetFailed          = 'HTTP Download failed.';
  SErrLoginFailed            = 'FTP LOGIN command failed.';
  SErrCWDFailed              = 'FTP CWD "%s" command failed.';
  SErrGETFailed              = 'FTP GET "%s" command failed.';
  SErrBrokenPackagesFound    = 'Found broken packages, run "fppkg fixbroken" first';
  SErrManifestNoSinglePackage = 'Manifest file "%s" does not contain exactly one package';

  SLogGeneratingFPMake       = 'Generating fpmake.pp';
  SLogNotCompilingFPMake     = 'Skipping compiling of fpmake.pp, fpmake executable already exists';
  SLogCommandLineAction      = 'Adding action from commandline: "%s %s"';
  SLogRunAction              = 'Action: "%s"';
  SLogExecute                = 'Executing: "%s %s"';
  SLogChangeDir              = 'CurrentDir: "%s"';
  SLogDownloading            = 'Downloading "%s" to "%s"';
  SLogUnzippping             = 'Unzipping "%s"';
  SLogZippping               = 'Zipping "%s"';
  SLogLoadingGlobalConfig    = 'Loaded global configuration from "%s"';
  SLogLoadingCompilerConfig  = 'Loading compiler configuration from "%s"';
  SLogLoadingFPMakeCompilerConfig = 'Loading compiler configuration for fpmake building from "%s"';
  SLogGeneratingGlobalConfig = 'Generating default global configuration in "%s"';
  SLogDetectedCompiler       = 'Detected compiler "%s" (version %s for %s)';
  SLogDetectedPrefix         = 'Detected %s prefix "%s"';
  SLogFPCDirEnv              = 'FPCDIR from environment setting "%s"';
  SLogGeneratingCompilerConfig  = 'Generating default compiler configuration in "%s"';
  SLogLoadingPackagesFile    = 'Loading available packages from "%s"';
  SLogLoadingMirrorsFile     = 'Loading available mirrors from "%s"';
  SLogFindInstalledPackages  = 'Finding installed packages in "%s"';
  SLogFoundFPMakeAddin       = 'Found FPMake-AddIn "%s"';
  SLogSavingStatusFile       = 'Saving local status to "%s"';
  SLogFPMKUnitDepVersion     = 'Checking for %s %s, installed %s, available %s';
  SLogFPMKUnitDepTooOld      = 'Minimum version of %s is not installed, using internal fpmkunit with limited functionality';
  SLogSelectedMirror         = 'Selected mirror "%s"';
  SLogStartLoadingConfFile   = 'Start loading configuration file "%s"';
  SLogUpgradingConfig        = 'Configuration file "%s" is updated with new configuration settings';
  SLogOldConfigFileFormat    = 'Configuration file is in an old format';
  SLogPackageDependency      = 'Dependency on package %s %s, installed %s, available %s  (%s)';
  SLogPackageChecksumChanged = 'Package %s needs to be rebuild, dependency %s is modified';
  SLogCheckBrokenDependenvies= 'Checking for broken dependencies';
  SLogFailedToCreateManifest = 'Failed to create manifest from fpmake.pp-file (%s) while scanning for available packages: %s';
  SLogUseInternalFpmkunit    = 'Fpmkunit not available, fallback to internal version.';

  SLogCfgHeader                      = 'Settings from configuration-files:';
  SLogCfgSectionHeader               = ' %s-section:';
  SLogGlobalCfgRemoteMirrorsURL      = '  RemoteMirrorsURL:      %s';
  SLogGlobalCfgRemoteRepository      = '  RemoteRepository:      %s';
  SLogGlobalCfgLocalRepository       = '  LocalRepository:       "%s" -> "%s"';
  SLogGlobalCfgBuildDir              = '  BuildDir:              "%s" -> "%s"';
  SLogGlobalCfgArchivesDir           = '  ArchivesDir:           "%s" -> "%s"';
  SLogGlobalCfgCompilerConfigDir     = '  CompilerConfigDir:     "%s" -> "%s"';
  SLogGlobalCfgDefaultCompilerConfig = '  DefaultCompilerConfig: "%s"';
  SLogGlobalCfgFPMakeCompilerConfig  = '  FPMakeCompilerConfig:  "%s"';
  SLogGlobalCfgDownloader            = '  Downloader:            %s';
  SLogCompilerCfgHeader           = 'Using %scompiler configuration file "%s":';
  SLogCompilerCfgCompiler         = ' Compiler:         "%s"';
  SLogCompilerCfgTarget           = ' Target:           %s';
  SLogCompilerCfgOptions          = ' Options:          "%s"';
  SLogCompilerCfgVersion          = ' Version:          %s';
  SLogCompilerCfgGlobalInstallDir = ' GlobalInstallDir: "%s" -> "%S"';
  SLogCompilerCfgLocalInstallDir  = ' LocalInstallDir:  "%s" -> "%s"';
  SLogCompilerCfgGlobalPrefix     = ' GlobalPrefix:     "%s" -> "%s"';
  SLogCompilerCfgLocalPrefix      = ' LocalPrefix:      "%s" -> "%s"';
  SLogRepositoryName              = '  Name:             %s';
  SLogRepositoryDescription       = '  Description:      "%s"';
  SLogRepositoryPath              = '  Dir:              "%s" -> "%s"';
  SLogRepositoryPrefix            = '  Prefix:           "%s" -> "%s"';

  SLogIncludeFile                 = '  IncludeFile:           "%s" -> "\%s"';
  SLogIncludeFileMask             = '  IncludeFileMask:       "%s" -> "\%s"';
  SLogIncludeFileDoesNotExist     = 'The log-file "%s" does not exist';
  SLogIncludeFileMaskDoesNotExist = 'The directory "%s" of the include-mask "%s" does not exist';

  SLogPackageInfoName             = 'Package:        %s';
  SLogPackageInfoVersion          = 'Version:        %s';
  SLogPackageInfoAuthor           = 'Author:         %s %s';
  SLogPackageInfoCategory         = 'Category:       %s';
  SLogPackageInfoLicense          = 'License:        %s';
  SLogPackageInfoWebsite          = 'Website:        %s';
  SLogPackageInfoOSes             = 'Supported OSes: %s';
  SLogPackageInfoCPUs             = 'Supported CPUs: %s';
  SLogPackageInfoDescription1     = 'Description:';
  SLogPackageInfoDescription2     = '%s';
  SLogPackageInfoDependencies1    = 'Dependencies:';
  SLogPackageInfoDependencies2    = '  %s %s';

  SDbgFound                  = 'Found';
  SDbgNotFound               = 'Not Found';
  SDbgDirectoryExists        = 'Directory "%s" %s';
  SDbgFileExists             = 'File "%s" %s';
  SDbgBackupFile             = 'Creating Backup File "%s"';
  SDbgPackageMultipleLocations = 'Multiple installations found for package %s, using installation "%s"';
  SDbgPackageDependencyOtherTarget  = 'Dependency on package %s is not for %s';
  SDbgObsoleteDependency     = 'Package %s depends on package %s which is not installed anymore';

  SProgrReinstallDependent   = 'Re-install packages which are dependent on just installed packages';
  SProgrInstallDependencies  = 'Install dependencies';
  SProgrDependenciesInstalled= 'Dependencies installed';
  SProgrDownloadPackage      = 'Downloading package %s version %s';

implementation

end.

