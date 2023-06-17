#!/bin/bash

if [ $# -lt 4 ]; then
    echo "Usage:
    ./organize.sh <submission folder> <target folder> <test folder> <answer folder> [-v] [-noexecute]
    -v: verbose
    -noexeute: do not execute code files"
else
    mkdir -p $2
if [ -d "$2" ]; then
    rm -rf "$2"/* >/dev/null 2>&1
fi
mkdir -p $2/C
mkdir -p $2/Java
mkdir -p $2/Python
verbose=false
noexecute=false
if [ $# -eq 5 ]; then
    if [ $5 = "-v" ]; then
        verbose=true
        noexecute=false
    elif [ $5 = "-noexecute" ]; then
        verbose=false
        noexecute=true
    fi
fi
if [ $# -eq 6 ]; then
    noexecute=true
    verbose=true
fi
cd submissions
#unzipping files in temp folder
mkdir temp
for zipfile in *.zip; do
    exdir="${zipfile%.zip}"
    mkdir -p "temp/$exdir"
    unzip -d "temp/$exdir" "$zipfile" 2>/dev/null
done
#organizing in c, java and python files
cd temp
temp=~/Offline/Workspace/$2
for item in *; do
    a=${item: -7}
    if $verbose; then
        echo "Organizing files of $a"
    fi
    find "$item" -name "*.c" \( -exec mkdir {} -p "$temp/C/$a" \; -o -exec true \; \) -exec mv {} "$temp/C/$a/main.c" \; -o -name "*.java" \( -exec mkdir {} -p "$temp/Java/$a" \; -o -exec true \; \) -exec mv {} "$temp/Java/$a/Main.java" \; -o -name "*.py" \( -exec mkdir {} -p "$temp/Python/$a" \; -o -exec true \; \) -exec mv {} "$temp/Python/$a/main.py" \;

done
cd ..
rm -fr temp
if ! $noexecute; then
    cd ..
    cd $2
    #creating .out and .class files
    find "." -name "*.c" -exec bash -c 'gcc "$0" -o "${0%.c}.out"' {} \;
    find "." -name "*.java" | while read -r file; do
    dir=$(dirname "$file")
    javac -d "$dir" "$file"
    done

#creating .csv file
touch "$temp/result.csv"
csv_file="$temp/result.csv"
echo "student_id,type,matched,not_matched" > "$csv_file"
ansfolder="~/Offline/Workspace/$4"

source_folders=("$temp/C" "$temp/Java" "$temp/Python")
for folder_path in "${source_folders[@]}"
do
    folder_name=$(basename "$folder_path")
    cd $folder_path
    for item in *; do
    name=$item
    if $verbose; then
    echo "Executing files of $name"
    fi
    matched=0
    not_matched=0
    cd ~/Offline/Workspace/$3
    for test in *; do
        a=${test:0: -4}
        b=${a: -1}
        cd "$temp/$folder_name/$name"
        touch "out$b.txt"
        if [ $folder_name = "C" ]; then
            ./main.out < ~/Offline/Workspace/$3/"$test" > "out$b.txt"
        elif [ $folder_name = "Java" ]; then
            java -cp "$temp/Java/$name" Main < ~/Offline/Workspace/$3/"$test" > "$temp/Java/$name/out$b.txt"
        elif [ $folder_name = "Python" ]; then
            python3 "$temp/Python/$name/main.py" < ~/Offline/Workspace/$3/"$test" > "$temp/Python/$name/out$b.txt"
        fi
        if diff "out$b.txt" ~/Offline/Workspace/$4/ans$b.txt >/dev/null; then
            matched=`expr $matched + 1`
        else
            not_matched=`expr $not_matched + 1`
        fi
    done
    echo "$name,$folder_name,$matched,$not_matched" >> "$csv_file"
done
done
fi
fi
