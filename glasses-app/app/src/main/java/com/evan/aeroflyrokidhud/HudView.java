package com.evan.aeroflyrokidhud;

import android.content.Context;
import android.graphics.*;
import android.view.View;
import java.util.Locale;

/** Monochrome HUD: hierarchy and shape remain readable on green waveguide displays. */
public final class HudView extends View {
    private static final int GREEN = Color.rgb(120, 255, 168);
    private final Paint ink = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final Path shape = new Path();
    private final RectF rect = new RectF();
    private final HudReceiver receiver = new HudReceiver();
    private boolean drawing;

    public HudView(Context context) { super(context); setBackgroundColor(Color.BLACK); }
    public void start() { drawing = true; receiver.start(); postInvalidate(); }
    public void stop() { drawing = false; receiver.stop(); }

    private void stroke(float width, int alpha) {
        ink.setColor(GREEN); ink.setAlpha(alpha); ink.setStrokeWidth(width);
        ink.setStyle(Paint.Style.STROKE);
    }
    private void fill(int alpha) {
        ink.setColor(GREEN); ink.setAlpha(alpha); ink.setStyle(Paint.Style.FILL);
    }
    private void label(Canvas c, String value, float x, float y, float size, int alpha, Paint.Align align, boolean numeric) {
        fill(alpha); ink.setTextAlign(align); ink.setTextSize(size);
        ink.setTypeface(Typeface.create(numeric ? "monospace" : "sans-serif", numeric || size >= 14 ? Typeface.BOLD : Typeface.NORMAL));
        c.drawText(value, x, y, ink);
    }
    private void text(Canvas c, String value, float x, float y, float size, int alpha) {
        label(c, value, x, y, size, alpha, Paint.Align.LEFT, false);
    }
    private void center(Canvas c, String value, float x, float y, float size, int alpha) {
        label(c, value, x, y, size, alpha, Paint.Align.CENTER, false);
    }
    private void rule(Canvas c, float x1, float y1, float x2, float y2, int alpha) {
        stroke(1, alpha); c.drawLine(x1, y1, x2, y2, ink);
    }
    private void box(Canvas c, float left, float top, float right, float bottom, int alpha) {
        rect.set(left, top, right, bottom);
        stroke(1, alpha); c.drawRoundRect(rect, 8, 8, ink);
    }
    private static String number(double value) {
        return Telemetry.finite(value) ? String.format(Locale.US, "%.0f", value) : "--";
    }

    @Override protected void onDraw(Canvas c) {
        super.onDraw(c);
        // Some glasses firmware ignores requested orientation. Keep a portrait
        // fallback instead of squeezing the wide dashboard into a narrow viewport.
        boolean wide = getWidth() >= getHeight();
        float designWidth = wide ? 640f : 480f;
        float designHeight = wide ? 480f : 640f;
        float scale = Math.min(getWidth() / designWidth, getHeight() / designHeight);
        c.save();
        c.translate((getWidth() - designWidth * scale) / 2, (getHeight() - designHeight * scale) / 2);
        c.scale(scale, scale);
        if (wide) renderWide(c, receiver.latest, HudReceiver.now());
        else render(c, receiver.latest, HudReceiver.now());
        c.restore();
        if (drawing) postInvalidateDelayed(50);
    }

    private void renderWide(Canvas c, Telemetry d, long now) {
        long age = d == null ? Long.MAX_VALUE : Math.max(0, now - d.receivedAt);
        boolean linked = age < 2500, live = linked && d.flight;
        String state = !linked ? receiver.status : live ? "实时飞行" : "等待游戏";
        text(c, "AERO / FLIGHT", 20, 30, 24, 255);
        center(c, d != null && d.test ? "测试数据" : "ROKID / WIDE", 378, 29, 13, 195);
        label(c, state, 620, 30, 20, 255, Paint.Align.RIGHT, false);

        String alert = !linked ? (d == null ? "寻找电脑 · 请启动 Aerofly" : "数据中断 · 正在重新连接") :
                !live ? "电脑已连接 · 等待新飞行数据" : d.alert;
        boolean urgent = live && (d.alert.equals("发动机火警") || d.alert.equals("主警告") || d.alert.endsWith("压力低"));
        box(c, 20, 44, 620, 80, urgent ? 255 : live ? 150 : 240);
        if (urgent) { rect.set(22, 46, 618, 78); stroke(1, 255); c.drawRoundRect(rect, 6, 6, ink); }
        fill(urgent && (now / 500) % 2 == 0 ? 110 : 255);
        c.drawCircle(35, 62, urgent ? 5 : 3, ink);
        center(c, alert, 324, 70, 21, 255);

        metric(c, 94, "空速  IAS", live ? d.ias : Double.NaN, "kt", false);
        metric(c, 192, "高度  ALT", live ? d.alt : Double.NaN, "ft", false);
        metric(c, 290, "垂直速度", live ? d.vs : Double.NaN, "ft/min", true);

        // Reposition each instrument without scaling its fonts or distorting it.
        double hdg = live ? d.heading : Double.NaN;
        label(c, Telemetry.finite(hdg) ? String.format(Locale.US, "%03.0f°", hdg % 360) : "---°",
                310, 112, 27, 255, Paint.Align.CENTER, true);
        text(c, "HDG", 370, 111, 13, 190);
        c.save(); c.translate(53, -81);
        horizon(c, d, live);
        c.restore();
        c.save(); c.translate(53, -80);
        gear(c, live ? d.gear : Double.NaN, live ? d.gearLabel() : "不可用", now);
        c.restore();
        c.save(); c.translate(0, -81);
        bar(c, 493, "襟翼", live ? d.flaps : Double.NaN);
        bar(c, 591, "减速板", live ? d.spoilers : Double.NaN);
        c.restore();

        rule(c, 20, 399, 620, 399, 110);
        text(c, "ENG / 油门", 20, 421, 17, 230);
        double throttle = live ? d.throttle : Double.NaN;
        label(c, Telemetry.finite(throttle) ? number(throttle * 100) + "%" : "--",
                620, 422, 26, 255, Paint.Align.RIGHT, true);
        for (int i = 0; i < 28; i++) {
            float x = 156 + i * 13.5f;
            if (Telemetry.finite(throttle) && throttle * 28 > i) {
                fill(215); c.drawRect(x, 410, x + 10, 419, ink);
            } else { stroke(1, 70); c.drawRect(x, 410, x + 10, 419, ink); }
        }
        rule(c, 20, 434, 620, 434, 110);
        String mode = linked ? d.mode : "重连";
        String frequency = linked ? String.format(Locale.US, "%.0f Hz", receiver.packetsPerSecond) : "-- Hz";
        String elapsed = d == null ? "--" : age < 1000 ? age + " ms" : String.format(Locale.US, "%.1f s", age / 1000.0);
        text(c, "连接 " + mode + "  /  " + frequency, 20, 452, 16, 240);
        label(c, "距上次收包 " + elapsed, 620, 452, 15, 215, Paint.Align.RIGHT, false);
        text(c, "PC " + receiver.peerName, 20, 473, 14, 195);
        label(c, "重试 " + receiver.reconnects + " · 异常包 " + receiver.badPackets, 620, 473, 14, 195, Paint.Align.RIGHT, false);
    }

    private void render(Canvas c, Telemetry d, long now) {
        long age = d == null ? Long.MAX_VALUE : Math.max(0, now - d.receivedAt);
        boolean linked = age < 2500, live = linked && d.flight;
        String state = !linked ? receiver.status : live ? "实时飞行" : "等待游戏";
        text(c, "AERO / FLIGHT", 20, 62, 24, 255);
        label(c, state, 460, 61, 20, 255, Paint.Align.RIGHT, false);
        text(c, "ROKID  ·  FLIGHT DECK", 20, 86, 13, 185);
        label(c, d != null && d.test ? "测试数据" : "HUD 02", 460, 85, 13, 200, Paint.Align.RIGHT, false);
        rule(c, 20, 94, 460, 94, 110);

        String alert = !linked ? (d == null ? "寻找电脑 · 请启动 Aerofly" : "数据中断 · 正在重新连接") :
                !live ? "电脑已连接 · 等待新飞行数据" : d.alert;
        boolean urgent = live && (d.alert.equals("发动机火警") || d.alert.equals("主警告") || d.alert.endsWith("压力低"));
        box(c, 20, 106, 460, 145, urgent ? 255 : live ? 150 : 240);
        if (urgent) { rect.set(22, 108, 458, 143); stroke(1, 255); c.drawRoundRect(rect, 6, 6, ink); }
        fill(urgent && (now / 500) % 2 == 0 ? 110 : 255); c.drawCircle(35, 125, urgent ? 5 : 3, ink);
        center(c, alert, 244, 133, 21, 255);

        metric(c, 180, "空速  IAS", live ? d.ias : Double.NaN, "kt", false);
        metric(c, 278, "高度  ALT", live ? d.alt : Double.NaN, "ft", false);
        metric(c, 376, "垂直速度", live ? d.vs : Double.NaN, "ft/min", true);
        heading(c, live ? d.heading : Double.NaN);
        horizon(c, d, live);
        gear(c, live ? d.gear : Double.NaN, live ? d.gearLabel() : "不可用", now);
        bar(c, 398, "襟翼", live ? d.flaps : Double.NaN);
        bar(c, 445, "减速板", live ? d.spoilers : Double.NaN);
        throttle(c, live ? d.throttle : Double.NaN);
        diagnostics(c, d, age, linked);
    }

    private void metric(Canvas c, float top, String name, double value, String unit, boolean signed) {
        box(c, 20, top, 144, top + 88, 100);
        rule(c, 20, top + 17, 20, top + 44, 255);
        text(c, name, 30, top + 22, 15, 220);
        String valueText = number(value);
        if (signed && Telemetry.finite(value))
            valueText = String.format(Locale.US, "%+.0f", Math.rint(value / 10) * 10);
        float size = valueText.length() > 6 ? 24 : valueText.length() > 5 ? 28 : valueText.length() > 4 ? 34 : 40;
        label(c, valueText, 134, top + 59, size, 255, Paint.Align.RIGHT, true);
        label(c, unit, 134, top + 80, 14, 200, Paint.Align.RIGHT, false);
    }

    private void heading(Canvas c, double heading) {
        boolean valid = Telemetry.finite(heading);
        label(c, valid ? String.format(Locale.US, "%03.0f°", heading % 360) : "---°",
                257, 173, 27, 255, Paint.Align.CENTER, true);
        c.save(); c.clipRect(152, 177, 365, 211);
        if (valid) {
            int base = (int) Math.floor(heading / 10) * 10;
            for (int mark = base - 40; mark <= base + 40; mark += 10) {
                float x = 257 + (float) (mark - heading) * 3;
                rule(c, x, 181, x, 187, 160);
                int degree = (mark % 360 + 360) % 360;
                String name = degree == 0 ? "N" : degree == 90 ? "E" : degree == 180 ? "S" : degree == 270 ? "W" : Integer.toString(degree / 10);
                if (x >= 167 && x <= 350) center(c, name, x, 203, 14, 210);
            }
        }
        c.restore();
        rule(c, 257, 179, 257, 187, 255);
    }

    private void horizon(Canvas c, Telemetry d, boolean live) {
        final float cx = 257, cy = 317, r = 105;
        shape.reset(); shape.addCircle(cx, cy, r - 4, Path.Direction.CW);
        c.save(); c.clipPath(shape); c.clipRect(cx - r, cy - r, cx + r, 381);
        if (live) {
            c.rotate((float) -d.bank, cx, cy);
            c.translate(0, (float) Math.max(-220, Math.min(220, d.pitch * 2.6)));
            rule(c, cx - 200, cy, cx + 200, cy, 230);
            for (int deg = -80; deg <= 80; deg += 10) {
                if (deg == 0) continue;
                float y = cy - deg * 2.6f;
                rule(c, cx - 28, y, cx + 28, y, 155);
                center(c, Integer.toString(Math.abs(deg)), cx - 43, y + 5, 13, 195);
                center(c, Integer.toString(Math.abs(deg)), cx + 43, y + 5, 13, 195);
            }
        }
        c.restore();
        stroke(1, 110); c.drawCircle(cx, cy, r, ink);
        for (int deg : new int[]{-60, -30, 0, 30, 60}) {
            c.save(); c.rotate(deg, cx, cy);
            rule(c, cx, cy - r, cx, cy - r + 7, 215); c.restore();
        }
        if (live) {
            stroke(2.5f, 255);
            shape.reset(); shape.moveTo(cx - 49, cy); shape.lineTo(cx - 13, cy);
            shape.lineTo(cx, cy + 9); shape.lineTo(cx + 13, cy); shape.lineTo(cx + 49, cy);
            c.drawPath(shape, ink);
            fill(255); c.drawCircle(cx, cy, 2, ink);
        } else center(c, "姿态数据不可用", cx, cy + 6, 18, 210);
        center(c, live ? String.format(Locale.US, "P %+.0f°   B %+.0f°", d.pitch, d.bank) : "P --   B --", cx, 402, 14, 230);
    }

    private void gear(Canvas c, double value, String state, long now) {
        box(c, 152, 434, 365, 474, 115);
        boolean valid = Telemetry.finite(value);
        boolean transition = valid && value > .02 && value < .98;
        int alpha = transition && (now / 500) % 2 == 0 ? 110 : 255;
        stroke(1.5f, valid ? alpha : 90);
        c.drawCircle(172, 458, 5, ink);
        c.drawLine(172, 443, 172, 453, ink); c.drawLine(165, 444, 179, 444, ink);
        text(c, "起落架", 187, 460, 16, 215);
        label(c, state, 352, 462, 23, alpha, Paint.Align.RIGHT, false);
    }

    private void bar(Canvas c, float x, String name, double value) {
        center(c, name, x, 203, 15, 230);
        boolean valid = Telemetry.finite(value);
        for (int i = 0; i < 16; i++) {
            float y = 399 - i * 11;
            if (valid && value * 16 > i) { fill(235); c.drawRect(x - 8, y, x + 8, y + 7, ink); }
            else { stroke(1, 80); c.drawRect(x - 8, y, x + 8, y + 7, ink); }
        }
        center(c, valid ? number(value * 100) : "--", x, 438, 23, 255);
        center(c, "%", x, 460, 14, 195);
    }

    private void throttle(Canvas c, double value) {
        rule(c, 20, 490, 460, 490, 110);
        text(c, "ENG / 油门", 20, 513, 17, 230);
        label(c, Telemetry.finite(value) ? number(value * 100) + "%" : "--",
                460, 513, 26, 255, Paint.Align.RIGHT, true);
        for (int i = 0; i < 32; i++) {
            float x = 20 + i * 13.8f;
            if (Telemetry.finite(value) && value * 32 > i) { fill(215); c.drawRect(x, 524, x + 10, 531, ink); }
            else { stroke(1, 70); c.drawRect(x, 524, x + 10, 531, ink); }
        }
    }

    private void diagnostics(Canvas c, Telemetry d, long age, boolean linked) {
        rule(c, 20, 548, 460, 548, 110);
        String mode = linked ? d.mode : "重连";
        String frequency = linked ? String.format(Locale.US, "%.0f Hz", receiver.packetsPerSecond) : "-- Hz";
        String elapsed = d == null ? "--" : age < 1000 ? age + " ms" : String.format(Locale.US, "%.1f s", age / 1000.0);
        text(c, "连接 " + mode + "  /  " + frequency, 20, 570, 16, 240);
        label(c, "距上次收包 " + elapsed, 460, 570, 15, 215, Paint.Align.RIGHT, false);
        text(c, "PC " + receiver.peerName, 20, 596, 14, 195);
        label(c, "重试 " + receiver.reconnects + " · 异常包 " + receiver.badPackets, 460, 596, 14, 195, Paint.Align.RIGHT, false);
    }
}
