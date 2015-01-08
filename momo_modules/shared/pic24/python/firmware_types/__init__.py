#This file aggregates all of the defined types that correspond with in-memory structures
#in the MoMo firmware.  Having these definitions allows you to interpret log statements
#containing those memory structures in a simple fashion.  
#
#NB All firmware types should be prefaced with fw_ or some other special prefix to distinguish them 
#from general purpose types. 

import scheduled_task as fw_scheduled_task
import task as fw_task