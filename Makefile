build: Bclock.obj Bclock.res
	link $? user32.lib gdi32.lib comdlg32.lib /out:Bclock.exe

Bclock.res: Bclock.rc
	rc $?

Bclock.obj: Bclock.c
	cl /c /W3 $?

clean:
	del *.obj Bclock.res Bclock.exe