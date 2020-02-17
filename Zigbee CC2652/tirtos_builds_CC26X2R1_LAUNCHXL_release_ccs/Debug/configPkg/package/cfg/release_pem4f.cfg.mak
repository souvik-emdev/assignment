# invoke SourceDir generated makefile for release.pem4f
release.pem4f: .libraries,release.pem4f
.libraries,release.pem4f: package/cfg/release_pem4f.xdl
	$(MAKE) -f C:\Users\Souvik\workspace_v9\tirtos_builds_CC26X2R1_LAUNCHXL_release_ccs/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\Souvik\workspace_v9\tirtos_builds_CC26X2R1_LAUNCHXL_release_ccs/src/makefile.libs clean

