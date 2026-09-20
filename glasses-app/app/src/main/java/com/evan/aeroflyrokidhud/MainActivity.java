package com.evan.aeroflyrokidhud;

import android.app.Activity;
import android.os.Bundle;
import android.view.WindowManager;

public final class MainActivity extends Activity {
    private HudView hud;

    @Override public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        hud = new HudView(this);
        setContentView(hud);
    }

    @Override public void onResume() {
        super.onResume();
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        hud.start();
    }

    @Override public void onPause() {
        hud.stop();
        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        super.onPause();
    }
}
