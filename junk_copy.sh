#!/bin/bash

# Michael Reilly
# junk.sh
# I pledge my honor that I have abided by the Stevens Honor System.
# 2/2/20

readonly JUNK_MAIN="/home/cs392/.junk";

if [ ! -d "$JUNK_MAIN" ]; then
	mkdir -p $JUNK_MAIN
fi

while getopts 'hlp' ListFILES; do
	case "${ListFILES}" in
		h)
		if [ "$#" -gt 1 ]; then
			echo "Error: Too many options enabled."
			cat << EOF
Usage: $(basename $0) [-hlp] [list of files]    
	-h: Display help.    
	-l: List junked files.    
	-p: Purge all files.    
	[list of files] with no other arguments to junk those files
EOF
			exit 1
		else
			cat << EOF
Usage: $(basename $0) [-hlp] [list of files]    
	-h: Display help.    
	-l: List junked files.    
	-p: Purge all files.    
	[list of files] with no other arguments to junk those files
EOF
			exit 0
		fi
			;;
		l)
		if [ "$#" -gt 1 ]; then
			echo "Error: Too many options enabled."
			cat << EOF
Usage: $(basename $0) [-hlp] [list of files]    
	-h: Display help.    
	-l: List junked files.    
	-p: Purge all files.    
	[list of files] with no other arguments to junk those files
EOF
			exit 1
		else
			for entry in $JUNK_MAIN
			do
				ls -lAF $entry
			done
			exit 0
		fi
			;;
		p)
		if [ "$#" -gt 1 ]; then
			echo "Error: Too many options enabled."
			cat << EOF
Usage: $(basename $0) [-hlp] [list of files]    
	-h: Display help.    
	-l: List junked files.    
	-p: Purge all files.    
	[list of files] with no other arguments to junk those files
EOF
		else
			rm -rf $JUNK_MAIN/* 2> /dev/null
			rm -rf $JUNK_MAIN/.* 2> /dev/null
			exit 0
		fi
			;;
		:)
			cat << EOF
Usage: $(basename $0) [-hlp] [list of files]    
	-h: Display help.    
	-l: List junked files.    
	-p: Purge all files.    
	[list of files] with no other arguments to junk those files
EOF
			exit 0
			;;
		?)
			echo "Error: unknown option '""$1""'." >&2
			cat << EOF
Usage: $(basename $0) [-hlp] [list of files]    
	-h: Display help.    
	-l: List junked files.    
	-p: Purge all files.    
	[list of files] with no other arguments to junk those files
EOF
			exit 1
			;;
	esac
done

for file in "$@"
	do
		if [ -f "/home/cs392/junk/${file}" ]; then
			mv "/home/cs392/junk/${file}" $JUNK_MAIN
		elif [ -a "/home/cs392/junk/${file}" ]; then
			shopt -s dotglob nullglob
			mv "/home/cs392/junk/${file}" $JUNK_MAIN
		else
			echo "Warning: '"$(basename /home/cs392/junk/${file})"' not found." >&2
		fi
done
exit 0
