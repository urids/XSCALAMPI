#!/bin/sh
#
# opal_get_version is created from opal_get_version.m4 and opal_get_version.m4sh.
#
# Copyright (c) 2004-2006 The Trustees of Indiana University and Indiana
#                         University Research and Technology
#                         Corporation.  All rights reserved.
# Copyright (c) 2004-2005 The University of Tennessee and The University
#                         of Tennessee Research Foundation.  All rights
#                         reserved.
# Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
#                         University of Stuttgart.  All rights reserved.
# Copyright (c) 2004-2005 The Regents of the University of California.
#                         All rights reserved.
# Copyright (c) 2008-2011 Cisco Systems, Inc.  All rights reserved.
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#



# OPAL_GET_VERSION(version_file, variable_prefix)
# -----------------------------------------------
# parse version_file for version information, setting
# the following shell variables:
#
#  prefix_VERSION
#  prefix_BASE_VERSION
#  prefix_MAJOR_VERSION
#  prefix_MINOR_VERSION
#  prefix_RELEASE_VERSION
#  prefix_GREEK_VERSION
#  prefix_WANT_REPO_REV
#  prefix_REPO_REV
#  prefix_RELEASE_DATE



srcfile="$1"
option="$2"

case "$option" in
    # svnversion can take a while to run.  If we don't need it, don't run it.
    --major|--minor|--release|--greek|--base|--help)
        ompi_ver_need_repo_rev=0
        ;;
    *)
        ompi_ver_need_repo_rev=1
esac


if test -z "$srcfile"; then
    option="--help"
else

    : ${ompi_ver_need_repo_rev=1}
    : ${svnversion_result=-1}

        if test -f "$srcfile"; then
        srcdir=`dirname $srcfile`
        ompi_vers=`sed -n "
	t clear
	: clear
	s/^major/OPAL_MAJOR_VERSION/
	s/^minor/OPAL_MINOR_VERSION/
	s/^release/OPAL_RELEASE_VERSION/
	s/^greek/OPAL_GREEK_VERSION/
	s/^want_repo_rev/OPAL_WANT_REPO_REV/
	s/^repo_rev/OPAL_REPO_REV/
	s/^date/OPAL_RELEASE_DATE/
	t print
	b
	: print
	p" < "$srcfile"`
	eval "$ompi_vers"

        # Only print release version if it isn't 0
        if test $OPAL_RELEASE_VERSION -ne 0 ; then
            OPAL_VERSION="$OPAL_MAJOR_VERSION.$OPAL_MINOR_VERSION.$OPAL_RELEASE_VERSION"
        else
            OPAL_VERSION="$OPAL_MAJOR_VERSION.$OPAL_MINOR_VERSION"
        fi
        OPAL_VERSION="${OPAL_VERSION}${OPAL_GREEK_VERSION}"
        OPAL_BASE_VERSION=$OPAL_VERSION

        if test $OPAL_WANT_REPO_REV -eq 1 && test $ompi_ver_need_repo_rev -eq 1 ; then
            if test "$svnversion_result" != "-1" ; then
                OPAL_REPO_REV=$svnversion_result
            fi
            if test "$OPAL_REPO_REV" = "-1" ; then

                d=`date '+%m-%d-%Y'`
                if test -d "$srcdir/.svn" ; then
                    OPAL_REPO_REV=r`svnversion "$srcdir"`
                    if test $? != 0; then
                        # The following is too long for Fortran
                        # OPAL_REPO_REV="unknown svn version (svnversion not found); $d"
                        OPAL_REPO_REV="? (no svnversion); $d"
                    fi
                elif test -d "$srcdir/.hg" ; then
                    # Check to see if we can find the hg command
                    # remember that $? reflects the status of the
                    # *last* command in a pipe change, so if "hg ..
                    # cut ..." runs and "hg" is not found, $? will
                    # reflect the status of "cut", not hg not being
                    # found.  So test for hg specifically first.
                    hg --version > /dev/null 2>&1
                    if test $? = 0; then
                        OPAL_REPO_REV=hg`hg -v -R "$srcdir" tip | grep ^changeset: | head -n 1 | cut -d: -f3`
                    else
                        # The following is too long for Fortran
                        # OPAL_REPO_REV="unknown hg version (hg not found); $d"
                        OPAL_REPO_REV="? (no hg); $d"
                    fi
                elif test -d "$srcdir/.git" ; then
                    # By the same logic as above, check to see if we
                    # can find the "git" command.
                    git --version > /dev/null 2>&1
                    if test $? = 0; then
                        OPAL_REPO_REV=git`git log -1 "$srcdir" | grep ^commit | awk '{ print OPAL }'`
                    else
                        # The following is too long for Fortran
                        # OPAL_REPO_REV="unknown hg version (hg not found); $d"
                        OPAL_REPO_REV="? (no git); $d"
                    fi
                fi
                if test "OPAL_REPO_REV" = ""; then
                    OPAL_REPO_REV="date$d"
                fi

            fi
            OPAL_VERSION="${OPAL_VERSION}${OPAL_REPO_REV}"
        fi
    fi


    if test "$option" = ""; then
	option="--full"
    fi
fi

case "$option" in
    --full|-v|--version)
	echo $OPAL_VERSION
	;;
    --major)
	echo $OPAL_MAJOR_VERSION
	;;
    --minor)
	echo $OPAL_MINOR_VERSION
	;;
    --release)
	echo $OPAL_RELEASE_VERSION
	;;
    --greek)
	echo $OPAL_GREEK_VERSION
	;;
    --repo-rev)
	echo $OPAL_REPO_REV
	;;
    --base)
        echo $OPAL_BASE_VERSION
        ;;
    --release-date)
        echo $OPAL_RELEASE_DATE
        ;;
    --all)
        echo ${OPAL_VERSION} ${OPAL_MAJOR_VERSION} ${OPAL_MINOR_VERSION} ${OPAL_RELEASE_VERSION} ${OPAL_GREEK_VERSION} ${OPAL_REPO_REV}
        ;;
    -h|--help)
	cat <<EOF
$0 <srcfile> <option>

<srcfile> - Text version file
<option>  - One of:
    --full         - Full version number
    --major        - Major version number
    --minor        - Minor version number
    --release      - Release version number
    --greek        - Greek (alpha, beta, etc) version number
    --repo-rev     - Repository version number
    --all          - Show all version numbers, separated by :
    --base         - Show base version number (no repo number)
    --release-date - Show the release date
    --help         - This message
EOF
        ;;
    *)
        echo "Unrecognized option $option.  Run $0 --help for options"
        ;;
esac

# All done

exit 0
