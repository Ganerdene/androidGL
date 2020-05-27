package com.example.myfirstnative;

import androidx.appcompat.app.AppCompatActivity;

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
                    //https://stackoverflow.com/questions/52329303/from-pixel-coordinate-to-opengl-coordinate
                    //2.0f * (x + 0.5f) / w - 1.0f;
                    final float normalizedX = (float) (2 * (event.getX() + 0.5) / v.getWidth() - 1);
                    final float normalizedY = -(float) (2 * (event.getY() + 0.5) / v.getHeight() - 1);

                    if (event.getAction() == MotionEvent.ACTION_DOWN){
                        Log.d("coordinate ", "x " + normalizedX + " Y " + normalizedY);
                        Log.d("non_coordinate ", "x " + (event.getX() / (float) v.getWidth())+ " Y " + ((event.getY() / (float) v.getHeight())));
                        touch(normalizedX, normalizedY);
                    }

                    return true;
                } else {
                    return false;
                }
            }
        });

    }

//    @Override
//    public boolean onTouchEvent(MotionEvent event)
//    {
//        float x = event.getX();
//        float y = event.getY();
//
//        final float normalizedX = (event.getX() / (float) v.getWidth()) * 2 - 1;
//        final float normalizedY = -((event.getY() / (float) v.getHeight()) * 2 - 1);
//
//        switch (event.getAction()) {
//            case MotionEvent.ACTION_DOWN:
//                Log.d("x y: ", x+ " "+y);
//                touch(x,y);
//
//                break;
//            case MotionEvent.ACTION_MOVE:
//            case MotionEvent.ACTION_UP:
//        }
//
//        return false;
//    }
    public static native void init(int width, int height);
    public static native void step();
    public static native void touch(float x, float y);
}
