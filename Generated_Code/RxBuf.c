/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : RxBuf.c
**     Project     : Laufkatze
**     Processor   : MKL25Z128VLK4
**     Component   : RingBuffer
**     Version     : Component 01.051, Driver 01.00, CPU db: 3.00.000
**     Repository  : My Components
**     Compiler    : GNU C Compiler
**     Date/Time   : 2018-02-23, 18:32, # CodeGen: 6
**     Abstract    :
**         This component implements a ring buffer for different integer data type.
**     Settings    :
**          Component name                                 : RxBuf
**          SDK                                            : MCUC1
**          Reentrant                                      : yes
**          Critical Section                               : CS1
**          Buffer Elements                                : 64
**          Element Size                                   : 1 Byte
**     Contents    :
**         Clear           - void RxBuf_Clear(void);
**         Put             - uint8_t RxBuf_Put(RxBuf_ElementType elem);
**         Get             - uint8_t RxBuf_Get(RxBuf_ElementType *elemP);
**         Peek            - uint8_t RxBuf_Peek(RxBuf_BufSizeType index, RxBuf_ElementType *elemP);
**         Update          - uint8_t RxBuf_Update(RxBuf_BufSizeType index, RxBuf_ElementType *elemP);
**         Putn            - uint8_t RxBuf_Putn(RxBuf_ElementType *elem, RxBuf_BufSizeType nof);
**         Getn            - uint8_t RxBuf_Getn(RxBuf_ElementType *buf, RxBuf_BufSizeType nof);
**         Compare         - uint8_t RxBuf_Compare(RxBuf_BufSizeType index, RxBuf_ElementType *elemP,...
**         Delete          - uint8_t RxBuf_Delete(void);
**         NofElements     - RxBuf_BufSizeType RxBuf_NofElements(void);
**         NofFreeElements - RxBuf_BufSizeType RxBuf_NofFreeElements(void);
**         Deinit          - void RxBuf_Deinit(void);
**         Init            - void RxBuf_Init(void);
**
**     * Copyright (c) 2014-2017, Erich Styger
**      * Web:         https://mcuoneclipse.com
**      * SourceForge: https://sourceforge.net/projects/mcuoneclipse
**      * Git:         https://github.com/ErichStyger/McuOnEclipse_PEx
**      * All rights reserved.
**      *
**      * Redistribution and use in source and binary forms, with or without modification,
**      * are permitted provided that the following conditions are met:
**      *
**      * - Redistributions of source code must retain the above copyright notice, this list
**      *   of conditions and the following disclaimer.
**      *
**      * - Redistributions in binary form must reproduce the above copyright notice, this
**      *   list of conditions and the following disclaimer in the documentation and/or
**      *   other materials provided with the distribution.
**      *
**      * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**      * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**      * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**      * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**      * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**      * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**      * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**      * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**      * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**      * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
** ###################################################################*/
/*!
** @file RxBuf.c
** @version 01.00
** @brief
**         This component implements a ring buffer for different integer data type.
*/         
/*!
**  @addtogroup RxBuf_module RxBuf module documentation
**  @{
*/         

/* MODULE RxBuf. */

#include "RxBuf.h"

#if RxBuf_CONFIG_REENTRANT
  #define RxBuf_DEFINE_CRITICAL() CS1_CriticalVariable()
  #define RxBuf_ENTER_CRITICAL()  CS1_EnterCritical()
  #define RxBuf_EXIT_CRITICAL()   CS1_ExitCritical()
#else
  #define RxBuf_DEFINE_CRITICAL() /* nothing */
  #define RxBuf_ENTER_CRITICAL()  /* nothing */
  #define RxBuf_EXIT_CRITICAL()   /* nothing */
#endif
static RxBuf_ElementType RxBuf_buffer[RxBuf_CONFIG_BUF_SIZE]; /* ring buffer */
static RxBuf_BufSizeType RxBuf_inIdx;  /* input index */
static RxBuf_BufSizeType RxBuf_outIdx; /* output index */
static RxBuf_BufSizeType RxBuf_inSize; /* size data in buffer */
/*
** ===================================================================
**     Method      :  RxBuf_Put (component RingBuffer)
**     Description :
**         Puts a new element into the buffer
**     Parameters  :
**         NAME            - DESCRIPTION
**         elem            - New element to be put into the buffer
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RxBuf_Put(RxBuf_ElementType elem)
{
  uint8_t res = ERR_OK;
  RxBuf_DEFINE_CRITICAL();

  RxBuf_ENTER_CRITICAL();
  if (RxBuf_inSize==RxBuf_CONFIG_BUF_SIZE) {
    res = ERR_TXFULL;
  } else {
    RxBuf_buffer[RxBuf_inIdx] = elem;
    RxBuf_inIdx++;
    if (RxBuf_inIdx==RxBuf_CONFIG_BUF_SIZE) {
      RxBuf_inIdx = 0;
    }
    RxBuf_inSize++;
  }
  RxBuf_EXIT_CRITICAL();
  return res;
}

/*
** ===================================================================
**     Method      :  RxBuf_Putn (component RingBuffer)
**     Description :
**         Put a number new element into the buffer.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * elem            - Pointer to new elements to be put into
**                           the buffer
**         nof             - number of elements
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RxBuf_Putn(RxBuf_ElementType *elem, RxBuf_BufSizeType nof)
{
  uint8_t res = ERR_OK;

  while(nof>0) {
    res = RxBuf_Put(*elem);
    if (res!=ERR_OK) {
      break;
    }
    elem++; nof--;
  }
  return res;
}

/*
** ===================================================================
**     Method      :  RxBuf_Get (component RingBuffer)
**     Description :
**         Removes an element from the buffer
**     Parameters  :
**         NAME            - DESCRIPTION
**       * elemP           - Pointer to where to store the received
**                           element
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RxBuf_Get(RxBuf_ElementType *elemP)
{
  uint8_t res = ERR_OK;
  RxBuf_DEFINE_CRITICAL();

  RxBuf_ENTER_CRITICAL();
  if (RxBuf_inSize==0) {
    res = ERR_RXEMPTY;
  } else {
    *elemP = RxBuf_buffer[RxBuf_outIdx];
    RxBuf_inSize--;
    RxBuf_outIdx++;
    if (RxBuf_outIdx==RxBuf_CONFIG_BUF_SIZE) {
      RxBuf_outIdx = 0;
    }
  }
  RxBuf_EXIT_CRITICAL();
  return res;
}

/*
** ===================================================================
**     Method      :  RxBuf_Getn (component RingBuffer)
**     Description :
**         Get a number elements into a buffer.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * buf             - Pointer to buffer where to store the
**                           elements
**         nof             - number of elements
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RxBuf_Getn(RxBuf_ElementType *buf, RxBuf_BufSizeType nof)
{
  uint8_t res = ERR_OK;

  while(nof>0) {
    res = RxBuf_Get(buf);
    if (res!=ERR_OK) {
      break;
    }
    buf++; nof--;
  }
  return res;
}

/*
** ===================================================================
**     Method      :  RxBuf_NofElements (component RingBuffer)
**     Description :
**         Returns the actual number of elements in the buffer.
**     Parameters  : None
**     Returns     :
**         ---             - Number of elements in the buffer.
** ===================================================================
*/
RxBuf_BufSizeType RxBuf_NofElements(void)
{
  return RxBuf_inSize;
}

/*
** ===================================================================
**     Method      :  RxBuf_NofFreeElements (component RingBuffer)
**     Description :
**         Returns the actual number of free elements/space in the
**         buffer.
**     Parameters  : None
**     Returns     :
**         ---             - Number of elements in the buffer.
** ===================================================================
*/
RxBuf_BufSizeType RxBuf_NofFreeElements(void)
{
  return (RxBuf_BufSizeType)(RxBuf_CONFIG_BUF_SIZE-RxBuf_inSize);
}

/*
** ===================================================================
**     Method      :  RxBuf_Init (component RingBuffer)
**     Description :
**         Initializes the data structure
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void RxBuf_Init(void)
{
  RxBuf_inIdx = 0;
  RxBuf_outIdx = 0;
  RxBuf_inSize = 0;
}

/*
** ===================================================================
**     Method      :  RxBuf_Clear (component RingBuffer)
**     Description :
**         Clear (empty) the ring buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void RxBuf_Clear(void)
{
  RxBuf_DEFINE_CRITICAL();

  RxBuf_ENTER_CRITICAL();
  RxBuf_Init();
  RxBuf_EXIT_CRITICAL();
}

/*
** ===================================================================
**     Method      :  RxBuf_Peek (component RingBuffer)
**     Description :
**         Returns an element of the buffer without removiing it.
**     Parameters  :
**         NAME            - DESCRIPTION
**         index           - Index of element. 0 peeks the top
**                           element, 1 the next, and so on.
**       * elemP           - Pointer to where to store the received
**                           element
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RxBuf_Peek(RxBuf_BufSizeType index, RxBuf_ElementType *elemP)
{
  uint8_t res = ERR_OK;
  int idx; /* index inside ring buffer */
  RxBuf_DEFINE_CRITICAL();

  RxBuf_ENTER_CRITICAL();
  if (index>=RxBuf_CONFIG_BUF_SIZE) {
    res = ERR_OVERFLOW; /* asking for an element outside of ring buffer size */
  } else if (index<RxBuf_inSize) {
    idx = (RxBuf_outIdx+index)%RxBuf_CONFIG_BUF_SIZE;
    *elemP = RxBuf_buffer[idx];
  } else { /* asking for an element which does not exist */
    res = ERR_RXEMPTY;
  }
  RxBuf_EXIT_CRITICAL();
  return res;
}

/*
** ===================================================================
**     Method      :  RxBuf_Compare (component RingBuffer)
**     Description :
**         Compares the elements in the buffer.
**     Parameters  :
**         NAME            - DESCRIPTION
**         index           - Index of element. 0 peeks the top
**                           element, 1 the next, and so on.
**       * elemP           - Pointer to elements to compare with
**         nof             - number of elements to compare
**     Returns     :
**         ---             - zero if elements are the same, -1 otherwise
** ===================================================================
*/
uint8_t RxBuf_Compare(RxBuf_BufSizeType index, RxBuf_ElementType *elemP, RxBuf_BufSizeType nof)
{
  uint8_t cmpResult = 0;
  uint8_t res;
  RxBuf_ElementType val;

  while(nof>0) {
    res = RxBuf_Peek(index, &val);
    if (res!=ERR_OK) { /* general failure? */
      cmpResult = (uint8_t)-1; /* no match */
      break;
    }
    if (val!=*elemP) { /* mismatch */
      cmpResult = (uint8_t)-1; /* no match */
      break;
    }
    elemP++; index++; nof--;
  }

  return cmpResult;
}

/*
** ===================================================================
**     Method      :  RxBuf_Deinit (component RingBuffer)
**     Description :
**         Driver de-initialization
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/**
void RxBuf_Deinit(void)
{
  ** Function is implemented as macro in the header file
}
*/
/*
** ===================================================================
**     Method      :  RxBuf_Delete (component RingBuffer)
**     Description :
**         Removes an element from the buffer
**     Parameters  : None
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RxBuf_Delete(void)
{
  uint8_t res = ERR_OK;
  RxBuf_DEFINE_CRITICAL();

  RxBuf_ENTER_CRITICAL();
  if (RxBuf_inSize==0) {
    res = ERR_RXEMPTY;
  } else {
    RxBuf_inSize--;
    RxBuf_outIdx++;
    if (RxBuf_outIdx==RxBuf_CONFIG_BUF_SIZE) {
      RxBuf_outIdx = 0;
    }
  }
  RxBuf_EXIT_CRITICAL();
  return res;
}

/*
** ===================================================================
**     Method      :  RxBuf_Update (component RingBuffer)
**     Description :
**         Updates the data of an element.
**     Parameters  :
**         NAME            - DESCRIPTION
**         index           - Index of element. 0 peeks the top
**                           element, 1 the next, and so on.
**       * elemP           - Pointer to where to store the received
**                           element
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
uint8_t RxBuf_Update(RxBuf_BufSizeType index, RxBuf_ElementType *elemP)
{
  uint8_t res = ERR_OK;
  int idx; /* index inside ring buffer */
  RxBuf_DEFINE_CRITICAL();

  RxBuf_ENTER_CRITICAL();
  if (index>=RxBuf_CONFIG_BUF_SIZE) {
    res = ERR_OVERFLOW; /* asking for an element outside of ring buffer size */
  } else if (index<RxBuf_inSize) {
    idx = (RxBuf_outIdx+index)%RxBuf_CONFIG_BUF_SIZE;
    RxBuf_buffer[idx] = *elemP; /* replace element */
  } else { /* asking for an element which does not exist */
    res = ERR_RXEMPTY;
  }
  RxBuf_EXIT_CRITICAL();
  return res;
}

/* END RxBuf. */

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
