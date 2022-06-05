module bkg_ram_unit
   #(
    parameter ADDR_WIDTH = 20,  // number of address bits
              DATA_WIDTH = 6    // color depth
   )
   (
    input  logic clk,
    input  logic we,
    input  logic [ADDR_WIDTH-1:0] addr_r,
    input  logic [ADDR_WIDTH-1:0] addr_w,
    input  logic [DATA_WIDTH-1:0] din,
    output logic [DATA_WIDTH-1:0] dout
   );

   // signal declaration
   logic [DATA_WIDTH-1:0] ram [0:(2**ADDR_WIDTH)-1];
   logic [DATA_WIDTH-1:0] data_reg;
   
   // master_reel_map.txt specifies the initial values of ram 
   initial 
      $readmemb("bkg_map_6.mif", ram);
      
   // body
   always_ff @(posedge clk)
   begin
      if (we)
         ram[addr_w] <= din;
      data_reg <= ram[addr_r];
   end
   assign dout = data_reg;
endmodule
