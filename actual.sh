#!/bin/bash

if [ $# -lt 4 ]; then
    echo "You must provide at least 4 arguments"
fi
mkdir -p $2
mkdir -p $2/C
mkdir -p $2/Java
mkdir -p $2/Python
verbose=false
noexecute=false
if [ $# -eq 5 ]; then
    if [ $5 = "-v" ]; then
        verbose=true
        # echo "-v true"
        noexecute=false
        # echo "-noexecute false"
    elif [ $5 = "-noexecute" ]; then
        verbose=false
        # echo "-v false"
        noexecute=true
        # echo "-noexecute true"
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
    unzip -dq "temp/$exdir" "$zipfile" 2>/dev/null
done
#organizing in c, java and python files
cd temp
temp=~/os/Workspace/$2
for item in *; do
    a=${item: -7}
    if $verbose; then
        echo "Organizing files of $a"
    fi
    # find "$item" -name "*.c" \( -exec mkdir {} ~/os/Workspace/targets/C/$a \; -o -exec true \; \) -exec cp {} ~/os/Workspace/targets/C/$a/ \; -exec mv {} ~/os/Workspace/targets/C/$a/main.c \; -o -name "*.java" \( -exec mkdir {} ~/os/Workspace/targets/Java/$a \; -o -exec true \; \) -exec cp {} ~/os/Workspace/targets/Java/$a/ \; -o -name "*.py" \( -exec mkdir {} ~/os/Workspace/targets/Python/$a \; -o -exec true \; \) -exec cp {} ~/os/Workspace/targets/Python/$a/ \;
    find "$item" -name "*.c" \( -exec mkdir {} -p "$temp/C/$a" \; -o -exec true \; \) -exec mv {} "$temp/C/$a/main.c" \; -o -name "*.java" \( -exec mkdir {} -p "$temp/Java/$a" \; -o -exec true \; \) -exec mv {} "$temp/Java/$a/Main.java" \; -o -name "*.py" \( -exec mkdir {} -p "$temp/Python/$a" \; -o -exec true \; \) -exec mv {} "$temp/Python/$a/main.py" \;

done
cd ..
rm -fr temp
if ! $noexecute; then
    cd ..
    cd $2
    #creating .out and .class files
    find "." -name "*.c" -exec bash -c 'gcc "$0" -o "${0%.c}.out"' {} \;
    # find "." -name "*.py" -exec python {} \;
    find "." -name "*.java" | while read -r file; do
    dir=$(dirname "$file")
    javac -d "$dir" "$file"
    done
# find "." -name "*.c" -exec bash -c 'gcc "$0" -o "${0%.c}.exe" && chmod +x "${0%.c}.exe"' {} \;


#creating .csv file
touch "$temp/result.csv"
csv_file="$temp/result.csv"
echo "student_id,type,matched,not_matched" > "$csv_file"
ansfolder="~/os/Workspace/$4"

cd "$temp/C"
for item in *; do
    name=$item
    if $verbose; then
    echo "Executing files of $name"
    fi
    matched=0
    not_matched=0
    cd ~/os/Workspace/tests
    for test in *; do
        a=${test:0: -4}
        b=${a: -1}
        cd "$temp/C/$name"
        touch "out$b.txt"
        ./main.out < ~/os/Workspace/tests/"$test" > "out$b.txt"
        if diff "out$b.txt" ~/os/Workspace/$4/ans$b.txt >/dev/null; then
            matched=`expr $matched + 1`
        else
            not_matched=`expr $not_matched + 1`
        fi
    done
    echo "$name,C,$matched,$not_matched" >> "$csv_file"
done

cd "$temp/Java"
for item in *; do
    name=$item
    if $verbose; then
    echo "Executing files of $name"
    fi
    matched=0
    not_matched=0
    cd ~/os/Workspace/tests
    for test in *; do
        a=${test:0: -4}
        b=${a: -1}
        cd "$temp/Java/$name"
        touch "out$b".txt
        java -cp "$temp/Java/$name" Main < ~/os/Workspace/tests/"$test" > "$temp/Java/$name/out$b.txt" 
        if diff "out$b.txt" ~/os/Workspace/$4/ans$b.txt >/dev/null; then
            matched=`expr $matched + 1`
        else
            not_matched=`expr $not_matched + 1`
        fi
    done
    echo "$name,Java,$matched,$not_matched" >> "$csv_file"
done

cd "$temp/Python"
for item in *; do
    name=$item
    if $verbose; then
    echo "Executing files of $name"
    fi
    matched=0
    not_matched=0
    cd ~/os/Workspace/tests
    for test in *; do
        a=${test:0: -4}
        b=${a: -1}
        cd "$temp/Python/$name"
        outputfile="out$b".txt
        touch "out$b".txt
        python3 "$temp/Python/$name/main.py" < ~/os/Workspace/tests/"$test" > "$temp/Python/$name/out$b.txt" 
        if diff "out$b.txt" ~/os/Workspace/$4/ans$b.txt >/dev/null; then
            matched=`expr $matched + 1`
        else
            not_matched=`expr $not_matched + 1`
        fi
    done
    echo "$name,Python,$matched,$not_matched" >> "$csv_file"
done
fi