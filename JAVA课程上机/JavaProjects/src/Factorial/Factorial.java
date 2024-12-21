package Factorial;

public class Factorial {
	static long last_ans=0;
	private static long calculate(int n) {
		long ans=1;
		for(int i=1;i<=n;++i) {
			ans*=i;
		}
		last_ans=ans;
		return ans;
	}
	private static long getFact() {
			return last_ans;
	}
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		System.out.println("5!="+calculate(5));
	}

}
