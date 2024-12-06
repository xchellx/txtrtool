/*
 * MIT License
 * 
 * Copyright (c) 2024 Yonder
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __TXTRTOOL_H__
#define __TXTRTOOL_H__
#include <stdlib.h>

typedef enum TTStatus {
    // No Error
    TTS_SUCCESS = EXIT_SUCCESS,
    // Unspecified Error
    TTS_ERROR = EXIT_FAILURE,
    // Unspecified Internal Error
    TTS_PROGERROR = EXIT_FAILURE + 1,
    // Internal Argument Error
    TTS_ARGERROR = EXIT_FAILURE + 2,
    // IO Error
    TTS_IOERROR = EXIT_FAILURE + 3,
    // File Format Error
    TTS_FMTERROR = EXIT_FAILURE + 4,
    // Memory Error
    TTS_MEMERROR = EXIT_FAILURE + 5
} TTStatus_t;

int main(int argc, char **argv);
#endif
