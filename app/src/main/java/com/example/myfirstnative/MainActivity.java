package com.example.myfirstnative;

import androidx.appcompat.app.AppCompatActivity;

import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Surface;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    static {
        System.loadLibrary("native-lib");
    }
    protected View graphicsView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        graphicsView = new View(getApplication());
        setContentView(graphicsView);

        graphicsView.setOnTouchListener(new android.view.View.OnTouchListener() {
            @Override
            public boolean onTouch(android.view.View v, MotionEvent event) {
                if (event != null) {
                    final float x = event.getX();
                    final float y = event.getY();
                    final int idx = event.getActionIndex();
                   if(event.getAction() == MotionEvent.ACTION_MOVE){
                       graphicsView.queueEvent(new Runnable() {
                           @Override
                           public void run() {
                               on_drag(x, y, idx);
                           }
                       });
                   }
                }
                return true;
            }
        });

    }
    public static native void init(int width, int height);
    public static native void onSurfaceChanged(int width, int height);
    public static native int loadTexture(AssetManager assetManager, String fileName);
    public static native void step();
    public static native void touch(float x, float y);
    public native void on_drag(float normalizedX, float normalizedY, int idx);
}
