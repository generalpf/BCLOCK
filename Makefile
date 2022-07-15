build: Base_cvt.obj Bclock.obj Bclock.res
	link $? Bclock.res user32.lib gdi32.lib comdlg32.lib /out:Bclock.exe

Bclock.res: Bclock.rc
	rc $?

%.obj: %.c
	cl /c /W3 $?

clean:
	del *.obj Bclock.res Bclock.exe