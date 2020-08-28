kernel-49.0.A.5.70
Sony Xperia L2 "ranger" (MT6737T).
Android 7.1.1 Kernel Version 3.18.35 
Intresting fact is that this kernel also contain's
Sony L1 pine _defconfigs (MT6737T).

Note: The /external dir is missing from here due to file size limits on github.
      You can get this source from Sony as a tar.gz https://developer.sony.com/file/download/open-source-archive-for-49-0-a-5-70/
.........

EDIT: "pine" (Sony L1) builds without any real issues using gcc aarch64-linux-android-4.9 but other gcc's seem to throw build errors.
The kernel I built using this source booted my TWRP just fine with no issues. 
