/*
 * Nulib2
 * Copyright (C) 2000 by Andy McFadden, All Rights Reserved.
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License, see the file COPYING.
 *
 * Add files to or update files in the archive.
 */
#include "Nulib2.h"

static NuError AddToArchive(NulibState* pState, NuArchive* pArchive);


/*
 * Add the specified files to a new or existing archive.
 */
NuError
DoAdd(NulibState* pState)
{
    NuError err;
    NuArchive* pArchive = nil;
    long flushStatus;

    assert(pState != nil);

    err = OpenArchiveReadWrite(pState);
    if (err != kNuErrNone)
        goto bail;

    pArchive = NState_GetNuArchive(pState);
    assert(pArchive != nil);

    NState_SetMatchCount(pState, 0);

    /* tell them about the list of files */
    err = AddToArchive(pState, pArchive);
    if (err != kNuErrNone)
        goto bail;

    /*(void)NuDebugDumpArchive(pArchive);*/

    if (!NState_GetMatchCount(pState))
        printf("%s: no records matched\n", gProgName);

bail:
    if (pArchive != nil) {
        NuError err2;

        #if 0
        if (err != kNuErrNone) {
            printf("Attempting to flush changes in spite of errors...\n");
            err = kNuErrNone;
        }
        #endif

        if (err == kNuErrNone) {
            err = NuFlush(pArchive, &flushStatus);
            if (err != kNuErrNone && err == kNuErrNone) {
                ReportError(err,
                    "Unable to flush archive changes (status=0x%04lx)",
                    flushStatus);
                NuAbort(pArchive);
            }
        } else {
            NuAbort(pArchive);
        }

        err2 = NuClose(pArchive);
        assert(err2 == kNuErrNone);
    }
    return err;
}


/*
 * Add the requested files to the specified archive.
 *
 * This just results in NuAddFile calls; the deferred write operation
 * isn't initiated.
 */
static NuError
AddToArchive(NulibState* pState, NuArchive* pArchive)
{
    NuError err = kNuErrNone;
    char* const* pSpec;
    ulong fileCount;
    int i;

    assert(pState != nil);
    assert(pArchive != nil);

    if (!NState_GetFilespecCount(pState)) {
        err = kNuErrSyntax;
        ReportError(err, "no files were specified");
    }

    fileCount = 0;

    pSpec = NState_GetFilespecPointer(pState);
    for (i = NState_GetFilespecCount(pState); i > 0; i--, pSpec++) {
        err = AddFile(pState, pArchive, *pSpec);
        if (err != kNuErrNone)
            goto bail;
    }

bail:
    return err;
}
