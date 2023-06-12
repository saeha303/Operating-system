#!/bin/bash

if [ $# -lt 4 ]; then
    echo "You must provide at least 4 arguments"
fi
mkdir $2
mkdir $2/C
mkdir $2/Java
mkdir $2/Python
# verbose=false
# noexecute=false
# if [ $# -eq 5 ]; then
# if [ $5 = "-v" ]; then
#     verbose=true
#     noexecute=true
# elif [ $5 = "-noexecute" ]; then
#     verbose=false
#     noexecute=true
# fi
# if [ $# -eq 6 ]; then
#     noexecute=true
#     verbose=true
# fi
# fi
cd submissions
#unzipping files in temp folder
mkdir temp
for zipfile in *.zip; do
    exdir="${zipfile%.zip}"
    mkdir "temp/$exdir"
    unzip -d "temp/$exdir" "$zipfile"
done
#organizing in c, java and python files
cd temp
for item in *; do
    a=${item: -7}
    # if $verbose; then
    #     echo "Organizing files of $a"
    # fi
    # find "$item" -name "*.c" \( -exec mkdir {} ~/os/Workspace/targets/C/$a \; -o -exec true \; \) -exec cp {} ~/os/Workspace/targets/C/$a/ \; -exec mv {} ~/os/Workspace/targets/C/$a/main.c \; -o -name "*.java" \( -exec mkdir {} ~/os/Workspace/targets/Java/$a \; -o -exec true \; \) -exec cp {} ~/os/Workspace/targets/Java/$a/ \; -o -name "*.py" \( -exec mkdir {} ~/os/Workspace/targets/Python/$a \; -o -exec true \; \) -exec cp {} ~/os/Workspace/targets/Python/$a/ \;
    find "$item" -name "*.c" \( -exec mkdir {} ~/os/Workspace/$2/C/$a \; -o -exec true \; \) -exec mv {} ~/os/Workspace/$2/C/$a/main.c \; -o -name "*.java" \( -exec mkdir {} ~/os/Workspace/$2/Java/$a \; -o -exec true \; \) -exec mv {} ~/os/Workspace/$2/Java/$a/Main.java \; -o -name "*.py" \( -exec mkdir {} ~/os/Workspace/$2/Python/$a \; -o -exec true \; \) -exec mv {} ~/os/Workspace/$2/Python/$a/main.py \;

done
cd ..
rm -fr temp
# if ! $noexecute; then
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
touch ~/os/Workspace/$2/result.csv
csv_file=~/os/Workspace/$2/result.csv
echo "student_id,type,matched,not_matched" > "$csv_file"
ansfolder="~/os/Workspace/$4"

cd ~/os/Workspace/$2/C
for item in *; do
    name=$item
    # echo "Executing files of $name"
    matched=0
    not_matched=0
    cd ~/os/Workspace/tests
    for test in *; do
        a=${test:0: -4}
        b=${a: -1}
        cd ~/os/Workspace/$2/C/$name
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

cd ~/os/Workspace/$2/Java
# cd Java
for item in *; do
    name=$item
    # echo "Executing files of $name"
    matched=0
    not_matched=0
    cd ~/os/Workspace/tests
    for test in *; do
        a=${test:0: -4}
        b=${a: -1}
        cd ~/os/Workspace/$2/Java/$name
        touch "out$b".txt
        java -cp ~/os/Workspace/$2/Java/$name Main < ~/os/Workspace/tests/"$test" > ~/os/Workspace/$2/Java/$name/"out$b".txt 
        if diff "out$b.txt" ~/os/Workspace/$4/ans$b.txt >/dev/null; then
            matched=`expr $matched + 1`
        else
            not_matched=`expr $not_matched + 1`
        fi
    done
    echo "$name,Java,$matched,$not_matched" >> "$csv_file"
done

cd ~/os/Workspace/$2/Python
for item in *; do
    name=$item
    # echo "Executing files of $name"
    matched=0
    not_matched=0
    cd ~/os/Workspace/tests
    for test in *; do
        a=${test:0: -4}
        b=${a: -1}
        cd ~/os/Workspace/$2/Python/$name
        outputfile="out$b".txt
        touch "out$b".txt
        python3 ~/os/Workspace/$2/Python/$name/main.py < ~/os/Workspace/tests/"$test" > ~/os/Workspace/$2/Python/$name/"out$b".txt 
        if diff "out$b.txt" ~/os/Workspace/$4/ans$b.txt >/dev/null; then
            matched=`expr $matched + 1`
        else
            not_matched=`expr $not_matched + 1`
        fi
    done
    echo "$name,Python,$matched,$not_matched" >> "$csv_file"
done
# fi