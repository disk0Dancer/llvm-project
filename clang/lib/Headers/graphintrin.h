/*===---- graphintrin.h - Graph target graphics intrinsics -----------------===*
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===*/

#ifndef __GRAPHINTRIN_H
#define __GRAPHINTRIN_H

#if !defined(__graph__)
#error "graphintrin.h is only available for the Graph target"
#endif

static __inline__ void __attribute__((__always_inline__, __nodebug__))
graph_init(int __width, int __height) {
  __builtin_graph_init(__width, __height);
}

static __inline__ void __attribute__((__always_inline__, __nodebug__))
graph_set_color(int __rgb) {
  __builtin_graph_set_color(__rgb);
}

static __inline__ void __attribute__((__always_inline__, __nodebug__))
graph_draw_pixel(int __x, int __y) {
  __builtin_graph_draw_pixel(__x, __y);
}

static __inline__ void __attribute__((__always_inline__, __nodebug__))
graph_flush(void) {
  __builtin_graph_flush();
}

#endif /* __GRAPHINTRIN_H */
