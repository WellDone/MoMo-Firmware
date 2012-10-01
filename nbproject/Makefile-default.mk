#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/DataLogger.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/DataLogger.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/main.o ${OBJECTDIR}/asm_routines.o ${OBJECTDIR}/rtcc.o ${OBJECTDIR}/interrupts.o ${OBJECTDIR}/serial.o ${OBJECTDIR}/serial_commands.o ${OBJECTDIR}/command_handlers.o ${OBJECTDIR}/ringbuffer.o ${OBJECTDIR}/utilities.o
POSSIBLE_DEPFILES=${OBJECTDIR}/main.o.d ${OBJECTDIR}/asm_routines.o.d ${OBJECTDIR}/rtcc.o.d ${OBJECTDIR}/interrupts.o.d ${OBJECTDIR}/serial.o.d ${OBJECTDIR}/serial_commands.o.d ${OBJECTDIR}/command_handlers.o.d ${OBJECTDIR}/ringbuffer.o.d ${OBJECTDIR}/utilities.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/main.o ${OBJECTDIR}/asm_routines.o ${OBJECTDIR}/rtcc.o ${OBJECTDIR}/interrupts.o ${OBJECTDIR}/serial.o ${OBJECTDIR}/serial_commands.o ${OBJECTDIR}/command_handlers.o ${OBJECTDIR}/ringbuffer.o ${OBJECTDIR}/utilities.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/DataLogger.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24F16KA101
MP_LINKER_FILE_OPTION=,-Tp24F16KA101.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/asm_routines.o: asm_routines.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/asm_routines.o.d 
	@${RM} ${OBJECTDIR}/asm_routines.o.ok ${OBJECTDIR}/asm_routines.o.err 
	@${FIXDEPS} "${OBJECTDIR}/asm_routines.o.d" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -I"/Users/timburke/Projects/DataLogger" -I"/Users/timburke/Projects/DataLogger/lib/inc/devices" asm_routines.s -o ${OBJECTDIR}/asm_routines.o -omf=elf -p=$(MP_PROCESSOR_OPTION) --defsym=__MPLAB_BUILD=1 --defsym=__MPLAB_DEBUG=1 --defsym=__ICD2RAM=1 --defsym=__DEBUG=1 --defsym=__MPLAB_DEBUGGER_PICKIT2=1 -g  -MD "${OBJECTDIR}/asm_routines.o.d" -I"/Users/timburke/Projects/DataLogger/lib/inc/devices"$(MP_EXTRA_AS_POST)
	
else
${OBJECTDIR}/asm_routines.o: asm_routines.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/asm_routines.o.d 
	@${RM} ${OBJECTDIR}/asm_routines.o.ok ${OBJECTDIR}/asm_routines.o.err 
	@${FIXDEPS} "${OBJECTDIR}/asm_routines.o.d" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE) -I"/Users/timburke/Projects/DataLogger" -I"/Users/timburke/Projects/DataLogger/lib/inc/devices" asm_routines.s -o ${OBJECTDIR}/asm_routines.o -omf=elf -p=$(MP_PROCESSOR_OPTION) --defsym=__MPLAB_BUILD=1 -g  -MD "${OBJECTDIR}/asm_routines.o.d" -I"/Users/timburke/Projects/DataLogger/lib/inc/devices"$(MP_EXTRA_AS_POST)
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o.ok ${OBJECTDIR}/main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"lib/inc" -I"lib/inc/devices" -Os -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c  
	
${OBJECTDIR}/rtcc.o: rtcc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/rtcc.o.d 
	@${RM} ${OBJECTDIR}/rtcc.o.ok ${OBJECTDIR}/rtcc.o.err 
	@${FIXDEPS} "${OBJECTDIR}/rtcc.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"lib/inc" -I"lib/inc/devices" -Os -MMD -MF "${OBJECTDIR}/rtcc.o.d" -o ${OBJECTDIR}/rtcc.o rtcc.c  
	
${OBJECTDIR}/interrupts.o: interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/interrupts.o.d 
	@${RM} ${OBJECTDIR}/interrupts.o.ok ${OBJECTDIR}/interrupts.o.err 
	@${FIXDEPS} "${OBJECTDIR}/interrupts.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"lib/inc" -I"lib/inc/devices" -Os -MMD -MF "${OBJECTDIR}/interrupts.o.d" -o ${OBJECTDIR}/interrupts.o interrupts.c  
	
${OBJECTDIR}/serial.o: serial.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/serial.o.d 
	@${RM} ${OBJECTDIR}/serial.o.ok ${OBJECTDIR}/serial.o.err 
	@${FIXDEPS} "${OBJECTDIR}/serial.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"lib/inc" -I"lib/inc/devices" -Os -MMD -MF "${OBJECTDIR}/serial.o.d" -o ${OBJECTDIR}/serial.o serial.c  
	
${OBJECTDIR}/serial_commands.o: serial_commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/serial_commands.o.d 
	@${RM} ${OBJECTDIR}/serial_commands.o.ok ${OBJECTDIR}/serial_commands.o.err 
	@${FIXDEPS} "${OBJECTDIR}/serial_commands.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"lib/inc" -I"lib/inc/devices" -Os -MMD -MF "${OBJECTDIR}/serial_commands.o.d" -o ${OBJECTDIR}/serial_commands.o serial_commands.c  
	
${OBJECTDIR}/command_handlers.o: command_handlers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/command_handlers.o.d 
	@${RM} ${OBJECTDIR}/command_handlers.o.ok ${OBJECTDIR}/command_handlers.o.err 
	@${FIXDEPS} "${OBJECTDIR}/command_handlers.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"lib/inc" -I"lib/inc/devices" -Os -MMD -MF "${OBJECTDIR}/command_handlers.o.d" -o ${OBJECTDIR}/command_handlers.o command_handlers.c  
	
${OBJECTDIR}/ringbuffer.o: ringbuffer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/ringbuffer.o.d 
	@${RM} ${OBJECTDIR}/ringbuffer.o.ok ${OBJECTDIR}/ringbuffer.o.err 
	@${FIXDEPS} "${OBJECTDIR}/ringbuffer.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"lib/inc" -I"lib/inc/devices" -Os -MMD -MF "${OBJECTDIR}/ringbuffer.o.d" -o ${OBJECTDIR}/ringbuffer.o ringbuffer.c  
	
${OBJECTDIR}/utilities.o: utilities.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/utilities.o.d 
	@${RM} ${OBJECTDIR}/utilities.o.ok ${OBJECTDIR}/utilities.o.err 
	@${FIXDEPS} "${OBJECTDIR}/utilities.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"lib/inc" -I"lib/inc/devices" -Os -MMD -MF "${OBJECTDIR}/utilities.o.d" -o ${OBJECTDIR}/utilities.o utilities.c  
	
else
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o.ok ${OBJECTDIR}/main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"lib/inc" -I"lib/inc/devices" -Os -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c  
	
${OBJECTDIR}/rtcc.o: rtcc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/rtcc.o.d 
	@${RM} ${OBJECTDIR}/rtcc.o.ok ${OBJECTDIR}/rtcc.o.err 
	@${FIXDEPS} "${OBJECTDIR}/rtcc.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"lib/inc" -I"lib/inc/devices" -Os -MMD -MF "${OBJECTDIR}/rtcc.o.d" -o ${OBJECTDIR}/rtcc.o rtcc.c  
	
${OBJECTDIR}/interrupts.o: interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/interrupts.o.d 
	@${RM} ${OBJECTDIR}/interrupts.o.ok ${OBJECTDIR}/interrupts.o.err 
	@${FIXDEPS} "${OBJECTDIR}/interrupts.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"lib/inc" -I"lib/inc/devices" -Os -MMD -MF "${OBJECTDIR}/interrupts.o.d" -o ${OBJECTDIR}/interrupts.o interrupts.c  
	
${OBJECTDIR}/serial.o: serial.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/serial.o.d 
	@${RM} ${OBJECTDIR}/serial.o.ok ${OBJECTDIR}/serial.o.err 
	@${FIXDEPS} "${OBJECTDIR}/serial.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"lib/inc" -I"lib/inc/devices" -Os -MMD -MF "${OBJECTDIR}/serial.o.d" -o ${OBJECTDIR}/serial.o serial.c  
	
${OBJECTDIR}/serial_commands.o: serial_commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/serial_commands.o.d 
	@${RM} ${OBJECTDIR}/serial_commands.o.ok ${OBJECTDIR}/serial_commands.o.err 
	@${FIXDEPS} "${OBJECTDIR}/serial_commands.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"lib/inc" -I"lib/inc/devices" -Os -MMD -MF "${OBJECTDIR}/serial_commands.o.d" -o ${OBJECTDIR}/serial_commands.o serial_commands.c  
	
${OBJECTDIR}/command_handlers.o: command_handlers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/command_handlers.o.d 
	@${RM} ${OBJECTDIR}/command_handlers.o.ok ${OBJECTDIR}/command_handlers.o.err 
	@${FIXDEPS} "${OBJECTDIR}/command_handlers.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"lib/inc" -I"lib/inc/devices" -Os -MMD -MF "${OBJECTDIR}/command_handlers.o.d" -o ${OBJECTDIR}/command_handlers.o command_handlers.c  
	
${OBJECTDIR}/ringbuffer.o: ringbuffer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/ringbuffer.o.d 
	@${RM} ${OBJECTDIR}/ringbuffer.o.ok ${OBJECTDIR}/ringbuffer.o.err 
	@${FIXDEPS} "${OBJECTDIR}/ringbuffer.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"lib/inc" -I"lib/inc/devices" -Os -MMD -MF "${OBJECTDIR}/ringbuffer.o.d" -o ${OBJECTDIR}/ringbuffer.o ringbuffer.c  
	
${OBJECTDIR}/utilities.o: utilities.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/utilities.o.d 
	@${RM} ${OBJECTDIR}/utilities.o.ok ${OBJECTDIR}/utilities.o.err 
	@${FIXDEPS} "${OBJECTDIR}/utilities.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"lib/inc" -I"lib/inc/devices" -Os -MMD -MF "${OBJECTDIR}/utilities.o.d" -o ${OBJECTDIR}/utilities.o utilities.c  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/DataLogger.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/DataLogger.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}        -Wl,--defsym=__MPLAB_BUILD=1,--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PICKIT2=1
else
dist/${CND_CONF}/${IMAGE_TYPE}/DataLogger.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/DataLogger.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}        -Wl,--defsym=__MPLAB_BUILD=1,--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION)
	${MP_CC_DIR}/pic30-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/DataLogger.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -omf=elf
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
