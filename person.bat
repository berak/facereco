: person remove ida
: person restore ida

if not '%1' == 'restore' goto remove_person
	for %%i in (datab\%2_*) do move "%%i" data
	goto done
:remove_person
	for %%i in (data\%2_*) do move "%%i" datab
:done