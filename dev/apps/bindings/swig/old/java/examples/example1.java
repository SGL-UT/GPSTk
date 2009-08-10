class example1 {
   public static void main(String argv[]) {

      System.loadLibrary("gpstk");
      
      DayTime dt = new DayTime();
      System.out.println("Hello wierdos");
      System.out.println(dt.JD());
      
   }
}
