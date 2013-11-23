import loghex
import logcaller
import logassert

#Map control bytes to statement handlers
statements = {
		#0: FinishLogging,
		1: loghex.LogHex,
		#2: LogString,
		3: logcaller.LogCaller,
		4: logassert.LogAssert
}