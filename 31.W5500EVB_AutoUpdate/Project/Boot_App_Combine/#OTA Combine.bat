cd /d %~dp0
if exist app.bin (
	if exist boot.bin (
		del combine.bin
		fsutil file createnew combine.bin 32768
		cat app.bin >> combine.bin
		hbin boot.bin combine.bin
	)
)
