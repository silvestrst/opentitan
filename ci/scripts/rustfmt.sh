#!/bin/bash
# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

# A wrapper around rustfmt, used for CI.
#
# Expects a single argument, which is the pull request's target branch
# (usually "master").

set -e

if [ $# != 1 ]; then
    echo >&2 "Usage: rustfmt.sh <tgt-branch>"
    exit 1
fi
tgt_branch="$1"

merge_base="$(git merge-base origin/$tgt_branch HEAD)" || {
    echo >&2 "Failed to find fork point for origin/$tgt_branch."
    exit 1
}
echo "Running Rust lint checks on files changed since $merge_base"

COMMITS="$(git log --pretty="format:%H" ${tgt_branch}..HEAD)"

echo "${COMMITS}"
for commit in "${COMMITS}"; do
    echo "${commit}"
    echo "$(         \
      git log                      \
        -1                         \
        --pretty="format:"         \
        --name-only                \
        --diff-filter=d            \
        ${commit}                  \
        -- "*.rs" ".toml"          \
      | sort --unique              \
      | grep -v "^$"
    )"
#    echo "${RUST_FILES_CHANGED}"
#    for file in "${RUST_FILES_CHANGED}"; do
#        echo "${commit} hello"
#        echo "${file}"
#        git show $commit:$file
#    cd "$(dirname ${file})"
#    echo "$(pwd)"
#    rustfmt --check "${file}"
#    cd --
#    done;
done

