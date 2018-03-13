#!/bin/bash

#
# This script attempts to determine the cpplint warnings of recently modified files.
# It exits with a failure code (1) if it detects that the warning number has increased.
#

LINTER=$(which cpplint)
MAX_ACCEPTABLE_SCORE="100"
TEMPFILE=/tmp/lint_${CI_JOB_ID}.out
if [ ! -e $LINTER ]; then
    echo "Unable to locate linter"
    exit 0
fi

# Step 1 - Determine which two branches to compare.
#          If on master, compare with previous, otherwise compare with origin/master.

CURRENT_BRANCH=$(git rev-parse --abbrev-ref HEAD)

if [ "$CURRENT_BRANCH" == "master" ]; then
    echo "Comparing against prior commit on master branch."
    COMPARE_COMMIT=$(git rev-parse HEAD~1)
else
    echo "Comparing against HEAD of master branch."
    COMPARE_COMMIT=$(git rev-parse origin/master)
fi

# Step 2 - Get a list of all files that are different between those two commits.
#          Leave with non-failure code if no code files were modified.
DIFF_FILES=$(git diff --name-only $CURRENT_BRANCH $COMPARE_COMMIT | grep -P -e '.*\.(cpp|cc|c|hpp|h)$')
if [ -z "${DIFF_FILES// }" ]; then
    echo "No code files changed."
    exit 0
fi
echo "Total files changed:"
echo "$DIFF_FILES"

# Step 3 - Find all of the files that exist in the current commit.
#          e.g. Don't scan files that have been deleted.
NEW_FILES=""
for NEW_FILE in $DIFF_FILES
do
    if [ -e $NEW_FILE ]; then
        NEW_FILES="$NEW_FILES $NEW_FILE"
    fi
done

# Step 4 - Generate LINTER score for the current branch.
#          If the list is empty, skip this step.
#          If no score is was generated, exit without failure.
if [ ! -z "${NEW_FILES// }" ]; then
    echo "Checking files after changes:"
#    echo "$NEW_FILES"

    echo "$LINTER $NEW_FILES | grep -oP \"(?<=Total errors found: ).*$\""
    echo $TEMPFILE
    $LINTER $NEW_FILES 2> $TEMPFILE
    NEW_SCORE=$(grep -oP "(?<=Total errors found: ).*$" $TEMPFILE)

    echo "New Score: ${NEW_SCORE// }."

    # Call a second time to output the results to the console... useful when debugging.
    cat $TEMPFILE
    rm $TEMPFILE
fi
if [ -z "${NEW_SCORE// }" ]; then
    echo "Unable to calculate new score."
    exit 0
fi

if [  $(echo $MAX_ACCEPTABLE_SCORE">="$NEW_SCORE | bc -l) -eq 1 ]; then
    echo "Score ($NEW_SCORE) is below the minimum required ($MAX_ACCEPTABLE_SCORE)."
    echo "No need to compare with previous."
    exit 0
fi


# Step 5 - Checkout the reference branch.
#          Exit without failure if that doesn't work. (Happens during testing.)
git checkout -q $COMPARE_COMMIT
if [ $? -ne 0 ];then
   echo "Cannot switch branches for comparison."
   exit 0
fi

# Step 6 - Find all of the files that exist in the reference commit.
#          e.g. Don't scan files didn't exist back then.
OLD_FILES=""
for OLD_FILE in $DIFF_FILES
do
     if [ -e $OLD_FILE ]; then
         OLD_FILES="$OLD_FILES $OLD_FILE"
     fi
done

# Step 7 - Generate LINTER score for the reference branch.
#          If the list is empty, skip this step.
#          If no score is was generated, exit without failure.

if [ ! -z "${OLD_FILES// }" ]; then
    echo "Checking files before changes:"
#    echo "$OLD_FILES"

    echo "$LINTER $OLD_FILES | grep -oP \"(?<=Total errors found: ).*$\""
    echo $TEMPFILE
    $LINTER $OLD_FILES 2> $TEMPFILE
    OLD_SCORE=$(grep -oP "(?<=Total errors found: ).*$" $TEMPFILE)
    rm $TEMPFILE
    git checkout -q $CURRENT_BRANCH
fi
if [ -z "${OLD_SCORE// }" ]; then
    echo "Unable to calculate old score."
    echo "New score is: $NEW_SCORE."
    git checkout -q $CURRENT_BRANCH
    exit 0
fi

# Step 8 - Display the score, and exit with failure if it has gone down.
echo "New=$NEW_SCORE Old=$OLD_SCORE"
if [ $(echo $OLD_SCORE">="$NEW_SCORE | bc -l) -eq 0 ];then
   echo "Files are getting worse!"
   exit 1
else
   echo "Files showed improvement."
   exit 0
fi

