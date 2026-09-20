package com.evan.aeroflyrokidhud;

import org.json.JSONObject;

/** Published atomically: drawing never observes half of a received packet. */
final class Telemetry {
    final double ias, alt, pitch, bank, heading, throttle, flaps, spoilers, vs, gear;
    final long session, sequence, receivedAt;
    final int version;
    final boolean flight, test;
    final String mode, alert;

    Telemetry(JSONObject j, long now) {
        version = j.optInt("v", 0);
        if (version != 1 && version != 2) throw new IllegalArgumentException("protocol");
        session = j.optLong("session", 0); sequence = j.optLong("seq", -1);
        if (version == 2 && (session <= 0 || sequence < 0)) throw new IllegalArgumentException("sequence");
        ias = value(j, "ias", 0, 10000); alt = value(j, "alt", -10000, 1000000);
        pitch = value(j, "pitch", -180, 180); bank = value(j, "bank", -360, 360);
        heading = value(j, "hdg", 0, 360);
        throttle = value(j, "thr", 0, 1); flaps = value(j, "flaps", 0, 1);
        spoilers = value(j, "spoilers", 0, 1);
        vs = value(j, "vs", -200000, 200000); gear = value(j, "gear", 0, 1);
        boolean core = finite(ias) && finite(alt) && finite(pitch) && finite(bank);
        if (version == 1 && !core) throw new IllegalArgumentException("missing flight data");
        flight = core && (version == 1 || j.optBoolean("flight", false));
        mode = version == 1 ? "旧版" : ("unicast".equals(j.optString("mode")) ? "单播" : "广播");
        test = j.optBoolean("test", false);
        String[] keys = {"fire", "master", "oil", "fuel", "hyd", "altAlert", "caution"};
        String[] labels = {"发动机火警", "主警告", "滑油压力低", "燃油压力低", "液压压力低", "高度警报", "注意"};
        String warning = "";
        boolean warningsKnown = true;
        for (int i = 0; i < keys.length; i++) {
            double v = value(j, keys[i], 0, 1);
            if (!finite(v)) warningsKnown = false;
            if (v > .5 && warning.isEmpty()) warning = labels[i];
        }
        alert = !warning.isEmpty() ? warning : spoilers > .05 ? "减速板展开" :
                warningsKnown ? "无活动警报" : "警报数据未完整提供";
        receivedAt = now;
    }
    static boolean finite(double value) { return !Double.isNaN(value) && !Double.isInfinite(value); }
    private static double value(JSONObject j, String key, double min, double max) {
        double v = j.optDouble(key, Double.NaN);
        return finite(v) && v >= min && v <= max ? v : Double.NaN;
    }
    String gearLabel() {
        return !finite(gear) ? "不可用" : gear <= .02 ? "收起" : gear >= .98 ? "放下" : "过渡";
    }
}
