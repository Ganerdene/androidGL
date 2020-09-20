package com.example.myfirstnative;

import androidx.annotation.LongDef;
import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;
import android.view.DragEvent;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    private float perv_x = 0.0f;
    private float perv_y = 0.0f;


    static {
        System.loadLibrary("native-lib");
    }

    protected View graphicsView;

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        graphicsView = new View(getApplication());
        setContentView(graphicsView);


        graphicsView.setOnClickListener(new android.view.View.OnClickListener() {


            @Override
            public void onClick(android.view.View v) {

            }
        });


        graphicsView.setOnTouchListener(new android.view.View.OnTouchListener() {
            boolean onDrag = false;

            @SuppressLint("ClickableViewAccessibility")
            @Override
            public boolean onTouch(android.view.View v, MotionEvent event) {
                final int action = event.getAction() & MotionEvent.ACTION_MASK;
                if (event != null) {
                    final float x = event.getX();
                    final float y = event.getY();
                    final int idx = event.getActionIndex();
                    Log.d("getActionIndex", "" + idx);
                    if (action == MotionEvent.ACTION_DOWN) {
                        graphicsView.queueEvent(new Runnable() {
                            @Override
                            public void run() {
                                on_drag(x, y, idx);
                            }
                        });
                        Log.d("onTouch onDrag ", "" + true);
                        onDrag = true;
                    }
                    if (action == MotionEvent.ACTION_MOVE) {



                    }
                    if(action == MotionEvent.ACTION_UP){
                        Log.d("onTouch onDrag ", "" + false);
                        onDrag = false;
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
