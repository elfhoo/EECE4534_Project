/**
 *@file audioTx.c
 *
 *@brief
 *  - receive audio samples from DMA
 *
 * Target:   TLL6527v1-0      
 * Compiler: VDSP++     Output format: VDSP++ "*.dxe"
 *
 * @author  Gunar Schirner
 *          Rohan Kangralkar
 * @date 03/15/2009
 *
 * LastChange:
 * $Id: audioTx.c 814 2013-03-12 03:59:36Z ovaskevi $
 *
 *******************************************************************************/
#include "audioTx.h"
#include "bufferPool_d.h"


/** Initialize audio tx
 *    - get pointer to buffer pool
 *    - register interrupt handler
 *    - initialize TX queue

 * Parameters:
 * @param pThis  pointer to own object
 * @param pBuffP  pointer to buffer pool to take and return chunks from
 * @param pIsrDisp   pointer to interrupt dispatcher to get ISR registered
 *
 * @return Zero on success.
 * Negative value on failure.
 */
int audioTx_init(audioTx_t *pThis, bufferPool_d_t *pBuffP)
{
    // paramter checking
    if ( NULL == pThis || NULL == pBuffP) {
        printf("[ATX]: Failed init\r\n");
        return -1;
    }
    
    // store pointer to buffer pool for later access     
    pThis->pBuffP       = pBuffP;

    pThis->pPending     = NULL; // nothing pending
    pThis->running      = 0;    // DMA turned off by default
    
    // init queue 
    pThis->queue = xQueueCreate(AUDIOTX_QUEUE_DEPTH, sizeof(chunk_d_t*));

    // note ISR registration done in _start
    
    printf("[ARX]: TX init complete\r\n");
    
    return 1;
}



/** start audio tx
 *   - empthy for now
 * Parameters:
 * @param pThis  pointer to own object
 *
 * @return Zero on success.
 * Negative value on failure.
 */
int audioTx_start(audioTx_t *pThis)
{
     
	/* initialize interrupt handler */

	XScuGic *pGic; // pointer to GIC interrupt driver
	pGic = prvGetInterruptControllerInstance(); // retrieve pointer to initialized instance

	// connect own interrupt handler
	XScuGic_Connect(pGic, XPS_FPGA15_INT_ID, (Xil_ExceptionHandler) audioTx_isr, (void*) pThis);

	// enable IRQ interrupt at GIC
	XScuGic_Enable(pGic, XPS_FPGA15_INT_ID);

	// define priority and trigger type for AXI Stream FIFO IRQ
	XScuGic_SetPriorityTriggerType(pGic, XPS_FPGA15_INT_ID, 0xA0, 0x3);

	/* Enable IRQ in processor core  */
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);

    return 1;
}



/** audio tx isr  (to be called from dispatcher) 
 *   - get chunk from tx queue
 *    - if valid, release old pending chunk to buffer pool 
 *    - configure DMA 
 *    - if not valide, configure DMA to replay same chunk again
 * Parameters:
 * @param pThis  pointer to own object
 *
 * @return None 
 */
void audioTx_isr(void *pThisArg)
{
    // create local casted pThis to avoid casting on every single access
	audioTx_t  *pThis = (audioTx_t*) pThisArg;

    /*TODO: place implementation here. See hints */


    /* check int type and clear interrupt */

    /* if queue is EMPTY
     *  - set signal that ISR is not running
     *  - return */

    /* receive pointer to chunk structure from Tx_queue,
     	Note: when ISR running, draw_wave_put should send the chunck to Tx_queue */

    /* how many samples does the chunk contain ? */

    /* check if sufficient space in device FIFO*/

	/* copy samples in chuck into FIFO */

    /* chunk has been copied into the FIFO, release chunk now. Return chunk to buffer pool. */


    return;
}



/** audio tx put
 *   copyies filled pChunk into the TX queue for transmission
 *    if queue is full, then chunk is dropped 
 * Parameters:
 * @param pThis  pointer to own object
 *
 * @return Zero on success.
 * Negative value on failure.
 */
int draw_wave_put(audioTx_t *pThis, chunk_d_t *pChunk, image_buf *pImg)
{
    
    if ( NULL == pThis || NULL == pChunk ) {
        printf("[TX]: Failed to put\r\n");
        return -1;
    }
    


    /* how many samples does the chunk contain ? */
	/* chunk max size is 512 bytes, but determined by bytes used*/
	int sample_data_size = pChunk->bytesUsed;
	int sample_size = sample_data_size/4;

	int counter = 0;

	   /* check if sufficient space in device FIFO*/
	while (*(u32*)(FIFO_BASE_ADDR + FIFO_TX_VAC) < sample_size )
	{
		vTaskDelay(1);
	}

		/* copy samples in chuck into FIFO */
	while (counter < sample_size)
	{
		u32 dataSample = pChunk->u32_buff[counter] << 16;
		*(u32*)(FIFO_BASE_ADDR + FIFO_TX_DATA) = dataSample;
		counter++;
		//write length
		*(u32*)(FIFO_BASE_ADDR + FIFO_TX_LENGTH) = 1;

	}


	/*********************************************************/
	/************draw hdmi view*******************************/





	//TODO: define the max xadc input value
	unsigned int xadc_max = 0xffff;
	//TODO: define the index couter


	    /* how many samples does the chunk contain ? */

    	//int sample_data_size = pChunk->bytesUsed;
    	//int sample_size = sample_data_size/4;



		int i;
		int j;

		//try to write to the matrix
		while (counter < sample_size && pImg -> horizontalCounter < 1920)
    	{
		u32 dataSample = pChunk->u32_buff[counter];
		int index_vertical;
		index_vertical = 1080 * (dataSample/xadc_max);
			for (i = 0; i<1080; i++)
			{
			//write to the index if it's the point to write to
				if (i == index_vertical)
				{
					pImg -> image[counter][i] = 0xffff;
				}
			//otherwise write blank image
				else
					pImg -> image[counter][i] = 0x0000;
			}

			pImg -> horizontalCounter ++;
			if (pImg -> horizontalCounter == 1980)
			{
				pImg -> horizontalCounter = 0;
				//image is full, transfer to the image buffer
				/*TODO: write to the HDMI buffer*/
			}
		}






    /* chunk has been copied into the FIFO, release chunk now. Return chunk to buffer pool. */

	bufferPool_d_release(pThis->pBuffP, pChunk);


    return 0;
}
