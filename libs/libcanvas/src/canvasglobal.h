#ifndef CANVAS_GLOBAL_H
#define CANVAS_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined(CANVAS_SYMBOLS)
	#define __libcanvas Q_DECL_EXPORT
#else
	#define __libcanvas Q_DECL_IMPORT
#endif

#endif
