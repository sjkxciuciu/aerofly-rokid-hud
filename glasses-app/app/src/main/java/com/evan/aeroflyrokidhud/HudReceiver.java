package com.evan.aeroflyrokidhud;

import java.net.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import org.json.JSONObject;

/** One socket owner per generation. Pausing invalidates the old worker permanently. */
final class HudReceiver {
    private static final byte[] HELLO = "AEROFLY_ROKID_HELLO_V1".getBytes(StandardCharsets.US_ASCII);
    private Session active;
    private volatile InetAddress knownPeer;
    volatile Telemetry latest;
    volatile String peerName = "--", status = "寻找电脑";
    volatile double packetsPerSecond;
    volatile int reconnects, badPackets;

    static long now() { return System.nanoTime() / 1000000; }
    private static final class Session {
        volatile boolean cancelled;
        DatagramSocket socket;
        Thread thread;
        synchronized void attach(DatagramSocket s) throws SocketException {
            if (cancelled) { s.close(); throw new SocketException("cancelled"); }
            socket = s;
        }
        synchronized void cancel() {
            cancelled = true;
            if (socket != null) socket.close();
            if (thread != null) thread.interrupt();
        }
    }
    synchronized void start() {
        if (active != null) return;
        Session s = new Session();
        active = s;
        s.thread = new Thread(() -> run(s), "AeroflyHudReceiver");
        s.thread.start();
    }
    synchronized void stop() {
        if (active != null) active.cancel();
        active = null;
        latest = null; packetsPerSecond = 0;
    }
    private synchronized boolean publish(Session s, Telemetry t, InetAddress from) {
        if (active != s || s.cancelled) return false;
        latest = t; knownPeer = from; peerName = from.getHostAddress(); status = "已连接";
        return true;
    }
    private synchronized void state(Session s, String text) {
        if (active == s) status = text;
    }
    private void run(Session s) {
        while (!s.cancelled) {
            try (DatagramSocket socket = new DatagramSocket(null)) {
                s.attach(socket);
                socket.setBroadcast(true);
                socket.setSoTimeout(250);
                socket.bind(new InetSocketAddress(49002));
                long opened = now(), lastHello = 0, lastBroadcast = 0, rateStart = opened;
                int count = 0;
                byte[] bytes = new byte[2048];
                while (!s.cancelled) {
                    long time = now();
                    Telemetry previous = latest;
                    long age = previous == null ? Long.MAX_VALUE : time - previous.receivedAt;
                    if (time - lastHello >= 1000) {
                        // Once discovered, keep registration alive by unicast as well.
                        if (knownPeer != null) send(socket, knownPeer);
                        lastHello = time;
                    }
                    if (time - lastBroadcast >= (age > 2500 ? 1000 : 10000)) {
                        discover(socket); lastBroadcast = time;
                    }
                    if (age > 2500) state(s, knownPeer == null ? "寻找电脑" : "正在重连");
                    if (time - rateStart >= 1000) {
                        packetsPerSecond = count * 1000.0 / (time - rateStart);
                        count = 0; rateStart = time;
                    }
                    // Rebind after prolonged silence, including network changes.
                    if (age > 6000 && time - opened > 6000) break;
                    DatagramPacket packet = new DatagramPacket(bytes, bytes.length);
                    try { socket.receive(packet); }
                    catch (SocketTimeoutException expected) { continue; }
                    if (s.cancelled) return;
                    try {
                        Telemetry t = new Telemetry(new JSONObject(new String(packet.getData(),
                                packet.getOffset(), packet.getLength(), StandardCharsets.UTF_8)), now());
                        previous = latest;
                        if (knownPeer != null && !knownPeer.equals(packet.getAddress()) &&
                                previous != null && now() - previous.receivedAt < 2500) continue;
                        if (previous != null && t.version == 2 && previous.session == t.session &&
                                t.sequence <= previous.sequence) continue; // duplicate/reordered beacon
                        if (publish(s, t, packet.getAddress())) count++;
                    } catch (Exception malformed) { badPackets++; }
                }
            } catch (SocketException e) {
                state(s, "接收端重试");
            } catch (java.io.IOException e) {
                state(s, "网络重试");
            }
            if (!s.cancelled) {
                reconnects++;
                try { Thread.sleep(500); }
                catch (InterruptedException e) { if (s.cancelled) return; }
            }
        }
    }
    private static void send(DatagramSocket socket, InetAddress address) {
        try { socket.send(new DatagramPacket(HELLO, HELLO.length, address, 49003)); }
        catch (java.io.IOException ignored) { /* Other discovery addresses still get tried. */ }
    }
    private static void discover(DatagramSocket socket) {
        try { send(socket, InetAddress.getByName("255.255.255.255")); }
        catch (UnknownHostException ignored) { }
        try {
            Enumeration<NetworkInterface> interfaces = NetworkInterface.getNetworkInterfaces();
            while (interfaces != null && interfaces.hasMoreElements()) {
                NetworkInterface nic = interfaces.nextElement();
                if (!nic.isUp() || nic.isLoopback()) continue;
                for (InterfaceAddress address : nic.getInterfaceAddresses()) {
                    if (address.getBroadcast() != null) send(socket, address.getBroadcast());
                }
            }
        } catch (SocketException ignored) { }
    }
}
