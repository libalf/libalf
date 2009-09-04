package de.libalf.dispatcher;

public class Test {
	public static void main(String[] args) {
		DispatcherSession session = new DispatcherSession("localhost", 24940);

		System.out.println(session.dispatchHelloCarsten(-1));
		System.out.println(session.dispatchReqVersion());
		System.out.println(session.dispatchReqCapa());
		System.out.println(session.dispatchHelloCarsten(23));
		System.out.println(session.dispatchReqVersion());
	}
}
