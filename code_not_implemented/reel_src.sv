module reel_src 
   #(
    parameter CD = 12,      // color depth
              ADDR = 16,    // number of address bits
              KEY_COLOR =0  // chroma key
   )
   (
    input  logic clk,
    input  logic [10:0] x, y,   // x-and  y-coordinate    
    input  logic [10:0] x0, y0, // origin of sprite 
    input  logic [4:0] ctrl,    // sprite control 
    // sprite ram write 
    input  logic we ,
    input  logic [ADDR-1:0] addr_w,
    input  logic [1:0] pixel_in,
    // pixel output
    output logic [CD-1:0] sprite_rgb
   );
   
   // localparam declaration
   localparam H_SIZE = 64; // horizontal size of sprite
   localparam V_SIZE = 192; // vertical size of sprite
   // signal delaration
   logic signed [11:0] xr, yr;  // relative x/y position
   logic in_region;
   logic [ADDR-1:0] addr_r;     //must change to reflect bigger sprite
   logic [3:0] sid;             // sprite id   
   logic [3:0] plt_code;        
   logic frame_tick, ani_tick;
   logic [3:0] c_next;        
   logic [3:0] c_reg;        
   logic [3:0] ani_next;        
   logic [3:0] ani_reg;        
   logic [10:0] x_d1_reg;
   logic [CD-1:0]  out_rgb;
   logic [CD-1:0] full_rgb, ghost_rgb;
   logic [CD-1:0] out_rgb_d1_reg;
   logic [1:0] gc_color_sel;        
   logic [3:0] gc_id_sel;        
   logic auto;        
   
   // body 
   assign gc_id_sel = ctrl[3:0];
   assign auto = ctrl[4];

   //******************************************************************
   // sprite RAM
   //******************************************************************
   // instantiate sprite RAM
   reel_ram_unit #(.ADDR_WIDTH(ADDR), .DATA_WIDTH(4)) reel_ram (
      .clk(clk), .we(we), .addr_w(addr_w), .din(pixel_in),
      .addr_r(addr_r), .dout(plt_code));
   assign addr_r = {sid, yr[5:0], xr[5:0]};
 
   //******************************************************************
   // ghost color control
   //******************************************************************

   // palette table
   always_comb
      case (plt_code)
        4'b0000:	full_rgb = 12'h000;
        4'b0001:	full_rgb = 12'hfff;
        4'b0010:	full_rgb = 12'hffd;
        4'b0011:	full_rgb = 12'h722;
        4'b0100:	full_rgb = 12'h944;
        4'b0101:	full_rgb = 12'h943;
        4'b0110:	full_rgb = 12'h566;
        4'b0111:	full_rgb = 12'hbbc;
        4'b1000:	full_rgb = 12'h556;
        4'b1001:	full_rgb = 12'hbcc;
        4'b1010:	full_rgb = 12'h760;
        4'b1011:	full_rgb = 12'hfc0;
        default: full_rgb = 12'hfff;   // white
      endcase   
   //******************************************************************
   // in-region circuit
   //******************************************************************
   // relative coordinate calculation
   assign xr = $signed({1'b0, x}) - $signed({1'b0, x0});
   assign yr = $signed({1'b0, y}) - $signed({1'b0, y0});
   // in-region comparison and multiplexing 
   assign in_region = ((0<= xr) && (xr<H_SIZE) && (0<=yr) && (yr<V_SIZE));
   assign out_rgb = in_region ? full_rgb : KEY_COLOR;
   //******************************************************************
   // animation timing control
   //******************************************************************
   // counters 
   always_ff @(posedge clk) begin
      x_d1_reg <= x;
      c_reg <= c_next;
      ani_reg <= ani_next;
   end
   assign c_next = (frame_tick && c_reg==3) ? 0 :
                   (frame_tick) ? c_reg + 1 :
                    c_reg; 
   //assign ani_next = (ani_tick) ? ani_reg + 1 : ani_reg;
   assign ani_next = (ani_tick) ? ((ani_reg == 12) ? 0 : ani_reg + 1) : ani_reg;
   // 60-Hz tick from fram counter 
   assign frame_tick = (x_d1_reg==0) && (x==1) && (y==0);
   // sprite animation id tick 
   assign ani_tick  = frame_tick  && (c_reg==0); 
   // sprite id selection
   assign sid = (auto) ? ani_reg : gc_id_sel;
   //******************************************************************
   // delay line (one clock) 
   //******************************************************************
   // output with a-stage delay line
   always_ff @(posedge clk) 
      out_rgb_d1_reg <= out_rgb;
   assign sprite_rgb = out_rgb_d1_reg;
endmodule
  