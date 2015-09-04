#!/usr/bin/env bash
for i in $(find $1 -name '*.h' -or -name '*.cpp' -or -name '*.glsl')
do
	if ! grep -q 'Copyright' "$i" 
	then
		cat COPYING.NOTICE "$i" >"$i.new" && mv "$i.new" "$i"
	fi
done