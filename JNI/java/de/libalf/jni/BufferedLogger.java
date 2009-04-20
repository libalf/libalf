package de.libalf.jni;

public class BufferedLogger extends LibALFObject {

	private final int INTERNAL = 0;
	private final int ERROR = 1;
	private final int WARN = 2;
	private final int INFO = 3;
	private final int DEBUG = 4;
	private final int ALGORITHM = 5;
	
	public static enum LoggerLevel {
		LOGGER_INTERNAL, LOGGER_ERROR, LOGGER_WARN, LOGGER_INFO, LOGGER_DEBUG, LOGGER_ALGORITHM
	}
	
	public static LoggerLevel DEFAULT_LOGGER_LEVEL = LoggerLevel.LOGGER_ALGORITHM;
	
	public BufferedLogger() {
		this.pointer = init();
	}
	
	private native long init();
	
	public BufferedLogger(LoggerLevel minimalLogLevel, boolean logAlgorithm) {
		switch (minimalLogLevel) {
		case LOGGER_INTERNAL:
			this.pointer = init(INTERNAL, logAlgorithm);
			break;
		case LOGGER_ERROR:
			this.pointer = init(ERROR, logAlgorithm);
			break;
		case LOGGER_WARN:
			this.pointer = init(WARN, logAlgorithm);
			break;			
		case LOGGER_INFO:
			this.pointer = init(INFO, logAlgorithm);
			break;
		case LOGGER_DEBUG:
			this.pointer = init(DEBUG, logAlgorithm);
			break;
		case LOGGER_ALGORITHM:
			this.pointer = init(ALGORITHM, logAlgorithm);
			break;
		default:
			this.pointer = init(ALGORITHM, logAlgorithm);
			break;
		}
	}
	
	private native long init(int logLevel, boolean logAlgorithm);
	
	public String receive_and_flush() {
		return receive_and_flush(this.pointer);
	}
	
	private native String receive_and_flush(long pointer);
}
