package com.dwtech.android.houyi.math;

public class HVector {
	public float x;
	public float y;
	public float z;

	public HVector() {

	}

	public HVector(float aX, float aY, float aZ) {
		x = aX;
		y = aY;
		z = aZ;
	}

	public HVector add(HVector v) {
		return new HVector(x + v.x, y + v.y, z + v.z);
	}

	public HVector substract(HVector v) {
		return new HVector(x - v.x, y - v.y, z - v.z);
	}

	public HVector divide(float factor) {
		if (factor == 0) {
			return this;
		}

		return new HVector(x / factor, y / factor, z / factor);
	}

	public double magnitude() {
		return Math.sqrt(x * x + y * y + z * z);
	}

	public void normalize() {
		double mag = magnitude();
		if (mag == 0){
			x = 0;
			y = 0;
			z = 0;
		}else{
			x /= mag;
			y /= mag;
			z /= mag;
		}
	}

	public static HVector crossProduct(HVector aVector1, HVector aVector2) {
		HVector Vector = new HVector();

		Vector.x = aVector1.y * aVector2.z - aVector1.z * aVector2.y;
		Vector.y = aVector1.z * aVector2.x - aVector1.x * aVector2.z;
		Vector.z = aVector1.x * aVector2.y - aVector1.y * aVector2.x;

		return Vector;
	}
	
	public void rotate(HVector aALong, double radian) {
		double x = this.x;
		double y = this.y;
		double z = this.z;
		double u = aALong.x;
		double v = aALong.y;
		double w = aALong.z;

		double ux = u * x;
		double uy = u * y;
		double uz = u * z;
		double vx = v * x;
		double vy = v * y;
		double vz = v * z;
		double wx = w * x;
		double wy = w * y;
		double wz = w * z;
		double sa = Math.sin(radian);
		double ca = Math.cos(radian);
		this.x = (float) (u * (ux + vy + wz)
				+ (x * (v * v + w * w) - u * (vy + wz)) * ca + (-wy + vz) * sa);
		this.y = (float) (v * (ux + vy + wz)
				+ (y * (u * u + w * w) - v * (ux + wz)) * ca + (wx - uz) * sa);
		this.z = (float) (w * (ux + vy + wz)
				+ (z * (u * u + v * v) - w * (ux + vy)) * ca + (-vx + uy) * sa);
	}
	
	public String toString(){
		return "X="+x+" Y="+y+" Z="+z;
	}
	
	public boolean equals(Object obj){
		return Math.abs(x - ((HVector)obj).x) < 0.0000000001 
		&& Math.abs(y - ((HVector)obj).y) < 0.00000000001
		&& Math.abs(z - ((HVector)obj).z) < 0.00000000001;
	}
	
	public boolean equalsOpp(HVector v){
		boolean res = (x + v.x < 0.000000000001
				&& y + v.y < 0.000000000001
				&& z + v.z < 0.000000000001);

		return res;
	}
}
