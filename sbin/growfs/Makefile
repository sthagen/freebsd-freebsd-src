# $TSHeader: src/sbin/growfs/Makefile,v 1.4 2000/12/05 19:45:24 tomsoft Exp $
#

.include <src.opts.mk>

PACKAGE=ufs
PROG=   growfs
SRCS=   growfs.c
MAN=	growfs.8

.if defined(GFSDBG)
SRCS+=	debug.c
CFLAGS+= -DFS_DEBUG
NO_WCAST_ALIGN= yes
.endif

LIBADD=	ufs util

HAS_TESTS=
SUBDIR.${MK_TESTS}+= tests

.include <bsd.prog.mk>
