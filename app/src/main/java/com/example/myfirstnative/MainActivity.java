package com.example.myfirstnative;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
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
    }
    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        float x = event.getX();
        float y = event.getY();

        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
            touch(x,y);
            case MotionEvent.ACTION_MOVE:
            case MotionEvent.ACTION_UP:
        }

        return false;
    }
    public static native void init(int width, int height);
    public static native void step();
    public static native void touch(float x, float y);
}
