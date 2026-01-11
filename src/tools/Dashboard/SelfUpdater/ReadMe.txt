SELF UPDATER

This tool is used as intermediate for application self updating:

1. the application who wants to "update itself" spawns SelfUpdater with the update parameters, and quits.
2. SelfUpdater waits for the application to end, copies the new version to the given location and restart it.


Command line syntax :
SelfUpdater.exe  CallingExeFullPath PathOfNewVersion


To trigger the self update from the application, use the following code (pathes should be included in double-quotes)
	_spawnl	(_P_DETACH, "SelfUpdater.exe", "SelfUpdater.exe", szCallingExeFullPath, szPathOfNewVersion, NULL);
	exit(0);
