package com.dwtech.android.houyi.math;

public class HMath {
	public static double degree2radian(double degree){
		return degree * Math.PI / 180;
	}
	
	public static double radian2degree(double degree){
		return degree * 180 / Math.PI;
	}
	
	public static float clamp(float v, float min, float max){
        if (v < min){
            return min;
        } else if (v > max){
            return max;
        } else {
            return v;
        }
    }
}
