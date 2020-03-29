#ifndef __DMERROR_H_
#define __DMERROR_H_

#define DM_E_OK 0 //Everything Okay
#define	DM_E_PARAM -1 //Input parameter invalid
#define	DM_E_MEM -2 //Out of memory
#define DM_E_WSTATE -3 //Request for object at wrong state
#define DM_E_EXIST -4 //Request for existed object
#define DM_E_NEXIST -5 //Request for non-existed object
#define DM_E_TIMEOUT -6 //Request timeout 
#define DM_E_TM_UNGRP -7 //Timer is not added to a group

#endif //__DMERROR_H_
