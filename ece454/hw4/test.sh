#!/bin/bash

echo ""
echo "Usage: test.sh <test-num>"
echo ""
echo "0 - all"
echo "1 - global locks"
echo "2 - TM"
echo "3 - list locks"
echo "4 - element locks"
echo "5 - reduction"

if [[ $1 -eq 1 ]] | [[ $1 -eq 0 ]];
then
    echo ""
    echo "Running global lock tests"
    echo ""
    echo "original implementation"
    echo ""
    /usr/bin/time randtrack 1 100 > og.outs
    sort -n og.outs > og.out
    rm og.outs

    echo ""
    echo "1 thread implementation"
    echo ""
    /usr/bin/time randtrack_global_lock 1 100 > glt1.outs
    sort -n glt1.outs > glt1.out
    rm glt1.outs

    echo ""
    echo "2 thread implementation"
    echo ""
    /usr/bin/time randtrack_global_lock 2 100 > glt2.outs
    sort -n glt2.outs > glt2.out
    rm glt2.outs

    echo ""
    echo "4 thread implementation"
    echo ""
    /usr/bin/time randtrack_global_lock 4  100 > glt4.outs
    sort -n glt4.outs > glt4.out
    rm glt4.outs

    diff og.out glt1.out > diff-gl-1.out
    diff og.out glt2.out > diff-gl-2.out
    diff og.out glt4.out > diff-gl-4.out
    echo ""
    if [[ -s diff-gl-1.out ]]
    then
        echo "thread 1 failed"
    else
        echo "thread 1 succeeded"
        rm diff-gl-1.out
    fi
    if [[ -s diff-gl-2.out ]]
    then
        echo "thread 2 failed"
    else
        echo "thread 2 succeeded"
        rm diff-gl-2.out
    fi
    if [[ -s diff-gl-4.out ]]
    then
        echo "thread 4 failed"
    else
        echo "thread 4 succeeded"
        rm diff-gl-4.out
    fi
fi

if [[ $1 -eq 2 ]] | [[ $1 -eq 0 ]];
then
    echo ""
    echo "Running TM tests"
    echo ""
    echo "1 thread implementation"
    echo ""
    /usr/bin/time randtrack_tm 1 100 > tm1.outs
    sort -n tm1.outs > tm1.out
    rm tm1.outs

    echo ""
    echo "2 thread implementation"
    echo ""
    /usr/bin/time randtrack_tm 2 100 > tm2.outs
    sort -n tm2.outs > tm2.out
    rm tm2.outs

    echo ""
    echo "4 thread implementation"
    echo ""
    /usr/bin/time randtrack_tm 4 100 > tm4.outs
    sort -n tm4.outs > tm4.out
    rm tm4.outs

    diff og.out tm1.out > diff-tm-1.out
    diff og.out tm2.out > diff-tm-2.out
    diff og.out tm4.out > diff-tm-4.out
    echo ""
    if [[ -s diff-tm-1.out ]]
    then
        echo "thread 1 failed"
    else
        echo "thread 1 succeeded"
        rm diff-tm-1.out
    fi
    if [[ -s diff-tm-2.out ]]
    then
        echo "thread 2 failed"
    else
        echo "thread 2 succeeded"
        rm diff-tm-2.out
    fi
    if [[ -s diff-tm-4.out ]]
    then
        echo "thread 4 failed"
    else
        echo "thread 4 succeeded"
        rm diff-tm-4.out
    fi
fi

if [[ $1 -eq 3 ]] | [[ $1 -eq 0 ]];
then
    echo ""
    echo "Running list lock tests"
    echo ""
    echo "1 thread implementation"
    echo ""
    /usr/bin/time randtrack_list_lock 1 100 > ll1.outs
    sort -n ll1.outs > ll1.out
    rm ll1.outs

    echo ""
    echo "2 thread implementation"
    echo ""
    /usr/bin/time randtrack_list_lock 2 100 > ll2.outs
    sort -n ll2.outs > ll2.out
    rm ll2.outs

    echo ""
    echo "4 thread implementation"
    echo ""
    /usr/bin/time randtrack_list_lock 4 100 > ll4.outs
    sort -n ll4.outs > ll4.out
    rm ll4.outs

    diff og.out ll1.out > diff-ll-1.out
    diff og.out ll2.out > diff-ll-2.out
    diff og.out ll4.out > diff-ll-4.out
    echo ""
    if [[ -s diff-ll-1.out ]]
    then
        echo "thread 1 failed"
    else
        echo "thread 1 succeeded"
        rm diff-ll-1.out
    fi
    if [[ -s diff-ll-2.out ]]
    then
        echo "thread 2 failed"
    else
        echo "thread 2 succeeded"
        rm diff-ll-2.out
    fi
    if [[ -s diff-ll-4.out ]]
    then
        echo "thread 4 failed"
    else
        echo "thread 4 succeeded"
        rm diff-ll-4.out
    fi
fi

if [[ $1 -eq 4 ]] | [[ $1 -eq 0 ]];
then
    echo ""
    echo "Running element lock tests"
    echo ""
    echo "1 thread implementation"
    echo ""
    /usr/bin/time randtrack_element_lock 1 100 > el1.outs
    sort -n el1.outs > el1.out
    rm el1.outs

    echo ""
    echo "2 thread implementation"
    echo ""
    /usr/bin/time randtrack_element_lock 2 100 > el2.outs
    sort -n el2.outs > el2.out
    rm el2.outs

    echo ""
    echo "4 thread implementation"
    echo ""
    /usr/bin/time randtrack_element_lock 4 100 > el4.outs
    sort -n el4.outs > el4.out
    rm el4.outs

    diff og.out el1.out > diff-el-1.out
    diff og.out el2.out > diff-el-2.out
    diff og.out el4.out > diff-el-4.out
    echo ""
    if [[ -s diff-el-1.out ]]
    then
        echo "thread 1 failed"
    else
        echo "thread 1 succeeded"
        rm diff-el-1.out
    fi
    if [[ -s diff-el-2.out ]]
    then
        echo "thread 2 failed"
    else
        echo "thread 2 succeeded"
        rm diff-el-2.out
    fi
    if [[ -s diff-el-4.out ]]
    then
        echo "thread 4 failed"
    else
        echo "thread 4 succeeded"
        rm diff-el-4.out
    fi
fi

if [[ $1 -eq 5 ]] | [[ $1 -eq 0 ]];
then
    echo ""
    echo "Running reduction tests"
    echo ""
    echo "1 thread implementation"
    echo ""
    /usr/bin/time randtrack_reduction 1 100 > r1.outs
    sort -n r1.outs > r1.out
    rm r1.outs

    echo ""
    echo "2 thread implementation"
    echo ""
    /usr/bin/time randtrack_reduction 2 100 > r2.outs
    sort -n r2.outs > r2.out
    rm r2.outs

    echo ""
    echo "4 thread implementation"
    echo ""
    /usr/bin/time randtrack_reduction 4 100 > r4.outs
    sort -n r4.outs > r4.out
    rm r4.outs

    diff og.out r1.out > diff-r-1.out
    diff og.out r2.out > diff-r-2.out
    diff og.out r4.out > diff-r-4.out
    echo ""
    if [[ -s diff-r-1.out ]]
    then
        echo "thread 1 failed"
    else
        echo "thread 1 succeeded"
        rm diff-r-1.out
    fi
    if [[ -s diff-r-2.out ]]
    then
        echo "thread 2 failed"
    else
        echo "thread 2 succeeded"
        rm diff-r-2.out
    fi
    if [[ -s diff-r-4.out ]]
    then
        echo "thread 4 failed"
    else
        echo "thread 4 succeeded"
        rm diff-r-4.out
    fi
fi

