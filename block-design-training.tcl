
################################################################
# This is a generated script based on design: design_1
#
# Though there are limitations about the generated script,
# the main purpose of this utility is to make learning
# IP Integrator Tcl commands easier.
################################################################

namespace eval _tcl {
proc get_script_folder {} {
   set script_path [file normalize [info script]]
   set script_folder [file dirname $script_path]
   return $script_folder
}
}
variable script_folder
set script_folder [_tcl::get_script_folder]

################################################################
# Check if script is running in correct Vivado version.
################################################################
set scripts_vivado_version 2024.1
set current_vivado_version [version -short]

if { [string first $scripts_vivado_version $current_vivado_version] == -1 } {
   puts ""
   if { [string compare $scripts_vivado_version $current_vivado_version] > 0 } {
      catch {common::send_gid_msg -ssname BD::TCL -id 2042 -severity "ERROR" " This script was generated using Vivado <$scripts_vivado_version> and is being run in <$current_vivado_version> of Vivado. Sourcing the script failed since it was created with a future version of Vivado."}

   } else {
     catch {common::send_gid_msg -ssname BD::TCL -id 2041 -severity "ERROR" "This script was generated using Vivado <$scripts_vivado_version> and is being run in <$current_vivado_version> of Vivado. Please run the script in Vivado <$scripts_vivado_version> then open the design in Vivado <$current_vivado_version>. Upgrade the design by running \"Tools => Report => Report IP Status...\", then run write_bd_tcl to create an updated script."}

   }

   return 1
}

################################################################
# START
################################################################

# To test this script, run the following commands from Vivado Tcl console:
# source design_1_script.tcl


# The design that will be created by this Tcl script contains the following 
# module references:
# emmetcore, emmetcore, emmetcore, emmetcore, FSM_Send_Root, FSM_Rec_Root, FSM_Mul_Root

# Please add the sources of those modules before sourcing this Tcl script.

# If there is no project opened, this script will create a
# project, but make sure you do not have an existing project
# <./myproj/project_1.xpr> in the current working folder.

set list_projs [get_projects -quiet]
if { $list_projs eq "" } {
   create_project project_1 myproj -part xcvm1802-vsva2197-2MP-e-S
   set_property BOARD_PART xilinx.com:vmk180:part0:3.2 [current_project]
}


# CHANGE DESIGN NAME HERE
variable design_name
set design_name design_1

# If you do not already have an existing IP Integrator design open,
# you can create a design using the following command:
#    create_bd_design $design_name

# Creating design if needed
set errMsg ""
set nRet 0

set cur_design [current_bd_design -quiet]
set list_cells [get_bd_cells -quiet]

if { ${design_name} eq "" } {
   # USE CASES:
   #    1) Design_name not set

   set errMsg "Please set the variable <design_name> to a non-empty value."
   set nRet 1

} elseif { ${cur_design} ne "" && ${list_cells} eq "" } {
   # USE CASES:
   #    2): Current design opened AND is empty AND names same.
   #    3): Current design opened AND is empty AND names diff; design_name NOT in project.
   #    4): Current design opened AND is empty AND names diff; design_name exists in project.

   if { $cur_design ne $design_name } {
      common::send_gid_msg -ssname BD::TCL -id 2001 -severity "INFO" "Changing value of <design_name> from <$design_name> to <$cur_design> since current design is empty."
      set design_name [get_property NAME $cur_design]
   }
   common::send_gid_msg -ssname BD::TCL -id 2002 -severity "INFO" "Constructing design in IPI design <$cur_design>..."

} elseif { ${cur_design} ne "" && $list_cells ne "" && $cur_design eq $design_name } {
   # USE CASES:
   #    5) Current design opened AND has components AND same names.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 1
} elseif { [get_files -quiet ${design_name}.bd] ne "" } {
   # USE CASES: 
   #    6) Current opened design, has components, but diff names, design_name exists in project.
   #    7) No opened design, design_name exists in project.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 2

} else {
   # USE CASES:
   #    8) No opened design, design_name not in project.
   #    9) Current opened design, has components, but diff names, design_name not in project.

   common::send_gid_msg -ssname BD::TCL -id 2003 -severity "INFO" "Currently there is no design <$design_name> in project, so creating one..."

   create_bd_design $design_name

   common::send_gid_msg -ssname BD::TCL -id 2004 -severity "INFO" "Making design <$design_name> as current_bd_design."
   current_bd_design $design_name

}

common::send_gid_msg -ssname BD::TCL -id 2005 -severity "INFO" "Currently the variable <design_name> is equal to \"$design_name\"."

if { $nRet != 0 } {
   catch {common::send_gid_msg -ssname BD::TCL -id 2006 -severity "ERROR" $errMsg}
   return $nRet
}

set bCheckIPsPassed 1
##################################################################
# CHECK IPs
##################################################################
set bCheckIPs 1
if { $bCheckIPs == 1 } {
   set list_check_ips "\ 
xilinx.com:ip:gt_bridge_ip:1.1\
xilinx.com:ip:gt_quad_base:1.1\
xilinx.com:ip:bufg_gt:1.0\
xilinx.com:ip:clk_wizard:1.0\
xilinx.com:ip:util_ds_buf:2.2\
xilinx.com:ip:xlconcat:2.1\
xilinx.com:ip:util_reduced_logic:2.0\
xilinx.com:ip:axis_vio:1.0\
xilinx.com:ip:emb_fifo_gen:1.0\
xilinx.com:ip:util_vector_logic:2.0\
user.org:user:root_hub_core:1.0\
xilinx.com:ip:axis_ila:1.2\
xilinx.com:ip:versal_cips:3.4\
xilinx.com:ip:axi_noc:1.1\
xilinx.com:ip:emb_mem_gen:1.0\
xilinx.com:ip:axi_bram_ctrl:4.1\
xilinx.com:ip:proc_sys_reset:5.0\
xilinx.com:ip:xlconstant:1.1\
"

   set list_ips_missing ""
   common::send_gid_msg -ssname BD::TCL -id 2011 -severity "INFO" "Checking if the following IPs exist in the project's IP catalog: $list_check_ips ."

   foreach ip_vlnv $list_check_ips {
      set ip_obj [get_ipdefs -all $ip_vlnv]
      if { $ip_obj eq "" } {
         lappend list_ips_missing $ip_vlnv
      }
   }

   if { $list_ips_missing ne "" } {
      catch {common::send_gid_msg -ssname BD::TCL -id 2012 -severity "ERROR" "The following IPs are not found in the IP Catalog:\n  $list_ips_missing\n\nResolution: Please add the repository containing the IP(s) to the project." }
      set bCheckIPsPassed 0
   }

}

##################################################################
# CHECK Modules
##################################################################
set bCheckModules 1
if { $bCheckModules == 1 } {
   set list_check_mods "\ 
emmetcore\
emmetcore\
emmetcore\
emmetcore\
FSM_Send_Root\
FSM_Rec_Root\
FSM_Mul_Root\
"

   set list_mods_missing ""
   common::send_gid_msg -ssname BD::TCL -id 2020 -severity "INFO" "Checking if the following modules exist in the project's sources: $list_check_mods ."

   foreach mod_vlnv $list_check_mods {
      if { [can_resolve_reference $mod_vlnv] == 0 } {
         lappend list_mods_missing $mod_vlnv
      }
   }

   if { $list_mods_missing ne "" } {
      catch {common::send_gid_msg -ssname BD::TCL -id 2021 -severity "ERROR" "The following module(s) are not found in the project: $list_mods_missing" }
      common::send_gid_msg -ssname BD::TCL -id 2022 -severity "INFO" "Please add source files for the missing module(s) above."
      set bCheckIPsPassed 0
   }
}

if { $bCheckIPsPassed != 1 } {
  common::send_gid_msg -ssname BD::TCL -id 2023 -severity "WARNING" "Will not continue with creation of design due to the error(s) above."
  return 3
}

##################################################################
# DESIGN PROCs
##################################################################



# Procedure to create entire design; Provide argument to make
# procedure reusable. If parentCell is "", will use root.
proc create_root_design { parentCell } {

  variable script_folder
  variable design_name

  if { $parentCell eq "" } {
     set parentCell [get_bd_cells /]
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj


  # Create interface ports
  set GT_Serial_0 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:gt_rtl:1.0 GT_Serial_0 ]

  set CLK_IN_D_0 [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:diff_clock_rtl:1.0 CLK_IN_D_0 ]

  set CLK_IN1_D_0 [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:diff_clock_rtl:1.0 CLK_IN1_D_0 ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {200000000} \
   ] $CLK_IN1_D_0

  set ddr4_dimm1 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:ddr4_rtl:1.0 ddr4_dimm1 ]

  set ddr4_dimm1_sma_clk [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:diff_clock_rtl:1.0 ddr4_dimm1_sma_clk ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {200000000} \
   ] $ddr4_dimm1_sma_clk

  set lpddr4_sma_clk1 [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:diff_clock_rtl:1.0 lpddr4_sma_clk1 ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {200321000} \
   ] $lpddr4_sma_clk1


  # Create ports

  # Create instance: gt_bridge_ip_0, and set properties
  set gt_bridge_ip_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:gt_bridge_ip:1.1 gt_bridge_ip_0 ]
  set_property -dict [list \
    CONFIG.BYPASS_MODE {true} \
    CONFIG.IP_LR0_SETTINGS {PRESET GTY-Aurora_64B66B RX_PAM_SEL NRZ TX_PAM_SEL NRZ TX_HD_EN 0 RX_HD_EN 0 RX_GRAY_BYP true TX_GRAY_BYP true RX_GRAY_LITTLEENDIAN true TX_GRAY_LITTLEENDIAN true RX_PRECODE_BYP\
true TX_PRECODE_BYP true RX_PRECODE_LITTLEENDIAN false TX_PRECODE_LITTLEENDIAN false INTERNAL_PRESET Aurora_64B66B GT_TYPE GTY GT_DIRECTION DUPLEX TX_LINE_RATE 16 TX_PLL_TYPE LCPLL TX_REFCLK_FREQUENCY\
100 TX_ACTUAL_REFCLK_FREQUENCY 100.000000000000 TX_FRACN_ENABLED false TX_FRACN_OVRD false TX_FRACN_NUMERATOR 0 TX_REFCLK_SOURCE R0 TX_DATA_ENCODING 64B66B_SYNC TX_USER_DATA_WIDTH 64 TX_INT_DATA_WIDTH\
64 TX_BUFFER_MODE 1 TX_BUFFER_BYPASS_MODE Fast_Sync TX_PIPM_ENABLE false TX_OUTCLK_SOURCE TXOUTCLKPMA TXPROGDIV_FREQ_ENABLE false TXPROGDIV_FREQ_SOURCE LCPLL TXPROGDIV_FREQ_VAL 250.000000 TX_DIFF_SWING_EMPH_MODE\
CUSTOM TX_64B66B_SCRAMBLER false TX_64B66B_ENCODER false TX_64B66B_CRC false TX_RATE_GROUP A RX_LINE_RATE 16 RX_PLL_TYPE LCPLL RX_REFCLK_FREQUENCY 100 RX_ACTUAL_REFCLK_FREQUENCY 100.000000000000 RX_FRACN_ENABLED\
false RX_FRACN_OVRD false RX_FRACN_NUMERATOR 0 RX_REFCLK_SOURCE R0 RX_DATA_DECODING 64B66B_SYNC RX_USER_DATA_WIDTH 64 RX_INT_DATA_WIDTH 64 RX_BUFFER_MODE 1 RX_OUTCLK_SOURCE RXOUTCLKPMA RXPROGDIV_FREQ_ENABLE\
false RXPROGDIV_FREQ_SOURCE LCPLL RXPROGDIV_FREQ_VAL 250.000000 RXRECCLK_FREQ_ENABLE false RXRECCLK_FREQ_VAL 0 INS_LOSS_NYQ 20 RX_EQ_MODE AUTO RX_COUPLING AC RX_TERMINATION PROGRAMMABLE RX_RATE_GROUP A\
RX_TERMINATION_PROG_VALUE 800 RX_PPM_OFFSET 0 RX_64B66B_DESCRAMBLER false RX_64B66B_DECODER false RX_64B66B_CRC false OOB_ENABLE false RX_COMMA_ALIGN_WORD 1 RX_COMMA_SHOW_REALIGN_ENABLE true PCIE_ENABLE\
false TX_LANE_DESKEW_HDMI_ENABLE false RX_COMMA_P_ENABLE false RX_COMMA_M_ENABLE false RX_COMMA_DOUBLE_ENABLE false RX_COMMA_P_VAL 0101111100 RX_COMMA_M_VAL 1010000011 RX_COMMA_MASK 0000000000 RX_SLIDE_MODE\
OFF RX_SSC_PPM 0 RX_CB_NUM_SEQ 0 RX_CB_LEN_SEQ 1 RX_CB_MAX_SKEW 1 RX_CB_MAX_LEVEL 1 RX_CB_MASK_0_0 false RX_CB_VAL_0_0 00000000 RX_CB_K_0_0 false RX_CB_DISP_0_0 false RX_CB_MASK_0_1 false RX_CB_VAL_0_1\
00000000 RX_CB_K_0_1 false RX_CB_DISP_0_1 false RX_CB_MASK_0_2 false RX_CB_VAL_0_2 00000000 RX_CB_K_0_2 false RX_CB_DISP_0_2 false RX_CB_MASK_0_3 false RX_CB_VAL_0_3 00000000 RX_CB_K_0_3 false RX_CB_DISP_0_3\
false RX_CB_MASK_1_0 false RX_CB_VAL_1_0 00000000 RX_CB_K_1_0 false RX_CB_DISP_1_0 false RX_CB_MASK_1_1 false RX_CB_VAL_1_1 00000000 RX_CB_K_1_1 false RX_CB_DISP_1_1 false RX_CB_MASK_1_2 false RX_CB_VAL_1_2\
00000000 RX_CB_K_1_2 false RX_CB_DISP_1_2 false RX_CB_MASK_1_3 false RX_CB_VAL_1_3 00000000 RX_CB_K_1_3 false RX_CB_DISP_1_3 false RX_CC_NUM_SEQ 0 RX_CC_LEN_SEQ 1 RX_CC_PERIODICITY 5000 RX_CC_KEEP_IDLE\
DISABLE RX_CC_PRECEDENCE ENABLE RX_CC_REPEAT_WAIT 0 RX_CC_VAL 00000000000000000000000000000000000000000000000000000000000000000000000000000000 RX_CC_MASK_0_0 false RX_CC_VAL_0_0 00000000 RX_CC_K_0_0 false\
RX_CC_DISP_0_0 false RX_CC_MASK_0_1 false RX_CC_VAL_0_1 00000000 RX_CC_K_0_1 false RX_CC_DISP_0_1 false RX_CC_MASK_0_2 false RX_CC_VAL_0_2 00000000 RX_CC_K_0_2 false RX_CC_DISP_0_2 false RX_CC_MASK_0_3\
false RX_CC_VAL_0_3 00000000 RX_CC_K_0_3 false RX_CC_DISP_0_3 false RX_CC_MASK_1_0 false RX_CC_VAL_1_0 00000000 RX_CC_K_1_0 false RX_CC_DISP_1_0 false RX_CC_MASK_1_1 false RX_CC_VAL_1_1 00000000 RX_CC_K_1_1\
false RX_CC_DISP_1_1 false RX_CC_MASK_1_2 false RX_CC_VAL_1_2 00000000 RX_CC_K_1_2 false RX_CC_DISP_1_2 false RX_CC_MASK_1_3 false RX_CC_VAL_1_3 00000000 RX_CC_K_1_3 false RX_CC_DISP_1_3 false PCIE_USERCLK2_FREQ\
250 PCIE_USERCLK_FREQ 250 RX_JTOL_FC 9.5980804 RX_JTOL_LF_SLOPE -20 RX_BUFFER_BYPASS_MODE Fast_Sync RX_BUFFER_BYPASS_MODE_LANE MULTI RX_BUFFER_RESET_ON_CB_CHANGE ENABLE RX_BUFFER_RESET_ON_COMMAALIGN DISABLE\
RX_BUFFER_RESET_ON_RATE_CHANGE ENABLE TX_BUFFER_RESET_ON_RATE_CHANGE ENABLE RESET_SEQUENCE_INTERVAL 0 RX_COMMA_PRESET NONE RX_COMMA_VALID_ONLY 0 } \
    CONFIG.IP_NO_OF_LANES {4} \
    CONFIG.IP_PRESET {GTY-Aurora_64B66B} \
  ] $gt_bridge_ip_0


  # Create instance: gt_quad_base_0, and set properties
  set gt_quad_base_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:gt_quad_base:1.1 gt_quad_base_0 ]
  set_property -dict [list \
    CONFIG.APB3_CLK_FREQUENCY {200.0} \
    CONFIG.BYPASS_DRC_58G {false} \
    CONFIG.CHANNEL_ORDERING {/gt_quad_base_0/TX0_GT_IP_Interface design_1_gt_bridge_ip_0_0./gt_bridge_ip_0/GT_TX0.0 /gt_quad_base_0/TX1_GT_IP_Interface design_1_gt_bridge_ip_0_0./gt_bridge_ip_0/GT_TX1.1\
/gt_quad_base_0/TX2_GT_IP_Interface design_1_gt_bridge_ip_0_0./gt_bridge_ip_0/GT_TX2.2 /gt_quad_base_0/TX3_GT_IP_Interface design_1_gt_bridge_ip_0_0./gt_bridge_ip_0/GT_TX3.3 /gt_quad_base_0/RX0_GT_IP_Interface\
design_1_gt_bridge_ip_0_0./gt_bridge_ip_0/GT_RX0.0 /gt_quad_base_0/RX1_GT_IP_Interface design_1_gt_bridge_ip_0_0./gt_bridge_ip_0/GT_RX1.1 /gt_quad_base_0/RX2_GT_IP_Interface design_1_gt_bridge_ip_0_0./gt_bridge_ip_0/GT_RX2.2\
/gt_quad_base_0/RX3_GT_IP_Interface design_1_gt_bridge_ip_0_0./gt_bridge_ip_0/GT_RX3.3} \
    CONFIG.GT_TYPE {GTY} \
    CONFIG.PORTS_INFO_DICT {LANE_SEL_DICT {PROT0 {RX0 RX1 RX2 RX3 TX0 TX1 TX2 TX3}} GT_TYPE GTY REG_CONF_INTF APB3_INTF BOARD_PARAMETER { }} \
    CONFIG.PROT0_ENABLE {true} \
    CONFIG.PROT0_GT_DIRECTION {DUPLEX} \
    CONFIG.PROT0_LR0_SETTINGS {RX_CB_MASK 00000000 RX_CB_VAL 00000000000000000000000000000000000000000000000000000000000000000000000000000000 RX_CB_K 00000000 RX_CB_DISP 00000000 RX_CC_MASK 00000000 RX_CC_K\
00000000 RX_CC_DISP 00000000 GT_DIRECTION DUPLEX TX_PAM_SEL NRZ TX_HD_EN 0 TX_GRAY_BYP true TX_GRAY_LITTLEENDIAN true TX_PRECODE_BYP true TX_PRECODE_LITTLEENDIAN false TX_LINE_RATE 16 TX_PLL_TYPE LCPLL\
TX_REFCLK_FREQUENCY 100 TX_ACTUAL_REFCLK_FREQUENCY 100.000000000000 TX_FRACN_ENABLED false TX_FRACN_OVRD false TX_FRACN_NUMERATOR 0 TX_REFCLK_SOURCE R0 TX_DATA_ENCODING 64B66B_SYNC TX_USER_DATA_WIDTH 64\
TX_INT_DATA_WIDTH 64 TX_BUFFER_MODE 1 TX_BUFFER_BYPASS_MODE Fast_Sync TX_PIPM_ENABLE false TX_OUTCLK_SOURCE TXOUTCLKPMA TXPROGDIV_FREQ_ENABLE false TXPROGDIV_FREQ_SOURCE LCPLL TXPROGDIV_FREQ_VAL 250.000000\
TX_DIFF_SWING_EMPH_MODE CUSTOM TX_64B66B_SCRAMBLER false TX_64B66B_ENCODER false TX_64B66B_CRC false TX_RATE_GROUP A TX_LANE_DESKEW_HDMI_ENABLE false TX_BUFFER_RESET_ON_RATE_CHANGE ENABLE PRESET GTY-Aurora_64B66B\
RX_PAM_SEL NRZ RX_HD_EN 0 RX_GRAY_BYP true RX_GRAY_LITTLEENDIAN true RX_PRECODE_BYP true RX_PRECODE_LITTLEENDIAN false INTERNAL_PRESET Aurora_64B66B RX_LINE_RATE 16 RX_PLL_TYPE LCPLL RX_REFCLK_FREQUENCY\
100 RX_ACTUAL_REFCLK_FREQUENCY 100.000000000000 RX_FRACN_ENABLED false RX_FRACN_OVRD false RX_FRACN_NUMERATOR 0 RX_REFCLK_SOURCE R0 RX_DATA_DECODING 64B66B_SYNC RX_USER_DATA_WIDTH 64 RX_INT_DATA_WIDTH\
64 RX_BUFFER_MODE 1 RX_OUTCLK_SOURCE RXOUTCLKPMA RXPROGDIV_FREQ_ENABLE false RXPROGDIV_FREQ_SOURCE LCPLL RXPROGDIV_FREQ_VAL 250.000000 RXRECCLK_FREQ_ENABLE false RXRECCLK_FREQ_VAL 0 INS_LOSS_NYQ 20 RX_EQ_MODE\
AUTO RX_COUPLING AC RX_TERMINATION PROGRAMMABLE RX_RATE_GROUP A RX_TERMINATION_PROG_VALUE 800 RX_PPM_OFFSET 0 RX_64B66B_DESCRAMBLER false RX_64B66B_DECODER false RX_64B66B_CRC false OOB_ENABLE false RX_COMMA_ALIGN_WORD\
1 RX_COMMA_SHOW_REALIGN_ENABLE true PCIE_ENABLE false RX_COMMA_P_ENABLE false RX_COMMA_M_ENABLE false RX_COMMA_DOUBLE_ENABLE false RX_COMMA_P_VAL 0101111100 RX_COMMA_M_VAL 1010000011 RX_COMMA_MASK 0000000000\
RX_SLIDE_MODE OFF RX_SSC_PPM 0 RX_CB_NUM_SEQ 0 RX_CB_LEN_SEQ 1 RX_CB_MAX_SKEW 1 RX_CB_MAX_LEVEL 1 RX_CB_MASK_0_0 false RX_CB_VAL_0_0 00000000 RX_CB_K_0_0 false RX_CB_DISP_0_0 false RX_CB_MASK_0_1 false\
RX_CB_VAL_0_1 00000000 RX_CB_K_0_1 false RX_CB_DISP_0_1 false RX_CB_MASK_0_2 false RX_CB_VAL_0_2 00000000 RX_CB_K_0_2 false RX_CB_DISP_0_2 false RX_CB_MASK_0_3 false RX_CB_VAL_0_3 00000000 RX_CB_K_0_3\
false RX_CB_DISP_0_3 false RX_CB_MASK_1_0 false RX_CB_VAL_1_0 00000000 RX_CB_K_1_0 false RX_CB_DISP_1_0 false RX_CB_MASK_1_1 false RX_CB_VAL_1_1 00000000 RX_CB_K_1_1 false RX_CB_DISP_1_1 false RX_CB_MASK_1_2\
false RX_CB_VAL_1_2 00000000 RX_CB_K_1_2 false RX_CB_DISP_1_2 false RX_CB_MASK_1_3 false RX_CB_VAL_1_3 00000000 RX_CB_K_1_3 false RX_CB_DISP_1_3 false RX_CC_NUM_SEQ 0 RX_CC_LEN_SEQ 1 RX_CC_PERIODICITY\
5000 RX_CC_KEEP_IDLE DISABLE RX_CC_PRECEDENCE ENABLE RX_CC_REPEAT_WAIT 0 RX_CC_VAL 00000000000000000000000000000000000000000000000000000000000000000000000000000000 RX_CC_MASK_0_0 false RX_CC_VAL_0_0 00000000\
RX_CC_K_0_0 false RX_CC_DISP_0_0 false RX_CC_MASK_0_1 false RX_CC_VAL_0_1 00000000 RX_CC_K_0_1 false RX_CC_DISP_0_1 false RX_CC_MASK_0_2 false RX_CC_VAL_0_2 00000000 RX_CC_K_0_2 false RX_CC_DISP_0_2 false\
RX_CC_MASK_0_3 false RX_CC_VAL_0_3 00000000 RX_CC_K_0_3 false RX_CC_DISP_0_3 false RX_CC_MASK_1_0 false RX_CC_VAL_1_0 00000000 RX_CC_K_1_0 false RX_CC_DISP_1_0 false RX_CC_MASK_1_1 false RX_CC_VAL_1_1\
00000000 RX_CC_K_1_1 false RX_CC_DISP_1_1 false RX_CC_MASK_1_2 false RX_CC_VAL_1_2 00000000 RX_CC_K_1_2 false RX_CC_DISP_1_2 false RX_CC_MASK_1_3 false RX_CC_VAL_1_3 00000000 RX_CC_K_1_3 false RX_CC_DISP_1_3\
false PCIE_USERCLK2_FREQ 250 PCIE_USERCLK_FREQ 250 RX_JTOL_FC 9.5980804 RX_JTOL_LF_SLOPE -20 RX_BUFFER_BYPASS_MODE Fast_Sync RX_BUFFER_BYPASS_MODE_LANE MULTI RX_BUFFER_RESET_ON_CB_CHANGE ENABLE RX_BUFFER_RESET_ON_COMMAALIGN\
DISABLE RX_BUFFER_RESET_ON_RATE_CHANGE ENABLE RESET_SEQUENCE_INTERVAL 0 RX_COMMA_PRESET NONE RX_COMMA_VALID_ONLY 0 GT_TYPE GTY} \
    CONFIG.PROT0_LR10_SETTINGS {NA NA} \
    CONFIG.PROT0_LR11_SETTINGS {NA NA} \
    CONFIG.PROT0_LR12_SETTINGS {NA NA} \
    CONFIG.PROT0_LR13_SETTINGS {NA NA} \
    CONFIG.PROT0_LR14_SETTINGS {NA NA} \
    CONFIG.PROT0_LR15_SETTINGS {NA NA} \
    CONFIG.PROT0_LR1_SETTINGS {NA NA} \
    CONFIG.PROT0_LR2_SETTINGS {NA NA} \
    CONFIG.PROT0_LR3_SETTINGS {NA NA} \
    CONFIG.PROT0_LR4_SETTINGS {NA NA} \
    CONFIG.PROT0_LR5_SETTINGS {NA NA} \
    CONFIG.PROT0_LR6_SETTINGS {NA NA} \
    CONFIG.PROT0_LR7_SETTINGS {NA NA} \
    CONFIG.PROT0_LR8_SETTINGS {NA NA} \
    CONFIG.PROT0_LR9_SETTINGS {NA NA} \
    CONFIG.PROT0_NO_OF_LANES {4} \
    CONFIG.PROT0_RX_MASTERCLK_SRC {RX0} \
    CONFIG.PROT0_TX_MASTERCLK_SRC {TX0} \
    CONFIG.PROT_OUTCLK_VALUES {CH0_RXOUTCLK 250 CH0_TXOUTCLK 250 CH1_RXOUTCLK 250 CH1_TXOUTCLK 250 CH2_RXOUTCLK 250 CH2_TXOUTCLK 250 CH3_RXOUTCLK 250 CH3_TXOUTCLK 250} \
    CONFIG.QUAD_USAGE {TX_QUAD_CH {TXQuad_0_/gt_quad_base_0 {/gt_quad_base_0 design_1_gt_bridge_ip_0_0.IP_CH0,design_1_gt_bridge_ip_0_0.IP_CH1,design_1_gt_bridge_ip_0_0.IP_CH2,design_1_gt_bridge_ip_0_0.IP_CH3\
MSTRCLK 1,0,0,0 IS_CURRENT_QUAD 1}} RX_QUAD_CH {RXQuad_0_/gt_quad_base_0 {/gt_quad_base_0 design_1_gt_bridge_ip_0_0.IP_CH0,design_1_gt_bridge_ip_0_0.IP_CH1,design_1_gt_bridge_ip_0_0.IP_CH2,design_1_gt_bridge_ip_0_0.IP_CH3\
MSTRCLK 1,0,0,0 IS_CURRENT_QUAD 1}}} \
    CONFIG.REFCLK_LIST {{/CLK_IN_D_0_clk_p[0]}} \
    CONFIG.REFCLK_STRING {HSCLK0_LCPLLGTREFCLK0 refclk_PROT0_R0_100_MHz_unique1 HSCLK1_LCPLLGTREFCLK0 refclk_PROT0_R0_100_MHz_unique1} \
    CONFIG.REG_CONF_INTF {APB3_INTF} \
    CONFIG.RX0_LANE_SEL {PROT0} \
    CONFIG.RX1_LANE_SEL {PROT0} \
    CONFIG.RX2_LANE_SEL {PROT0} \
    CONFIG.RX3_LANE_SEL {PROT0} \
    CONFIG.TX0_LANE_SEL {PROT0} \
    CONFIG.TX1_LANE_SEL {PROT0} \
    CONFIG.TX2_LANE_SEL {PROT0} \
    CONFIG.TX3_LANE_SEL {PROT0} \
  ] $gt_quad_base_0

  set_property -dict [list \
    CONFIG.CHANNEL_ORDERING.VALUE_MODE {auto} \
    CONFIG.GT_TYPE.VALUE_MODE {auto} \
    CONFIG.PROT0_ENABLE.VALUE_MODE {auto} \
    CONFIG.PROT0_GT_DIRECTION.VALUE_MODE {auto} \
    CONFIG.PROT0_LR0_SETTINGS.VALUE_MODE {auto} \
    CONFIG.PROT0_LR10_SETTINGS.VALUE_MODE {auto} \
    CONFIG.PROT0_LR11_SETTINGS.VALUE_MODE {auto} \
    CONFIG.PROT0_LR12_SETTINGS.VALUE_MODE {auto} \
    CONFIG.PROT0_LR13_SETTINGS.VALUE_MODE {auto} \
    CONFIG.PROT0_LR14_SETTINGS.VALUE_MODE {auto} \
    CONFIG.PROT0_LR15_SETTINGS.VALUE_MODE {auto} \
    CONFIG.PROT0_LR1_SETTINGS.VALUE_MODE {auto} \
    CONFIG.PROT0_LR2_SETTINGS.VALUE_MODE {auto} \
    CONFIG.PROT0_LR3_SETTINGS.VALUE_MODE {auto} \
    CONFIG.PROT0_LR4_SETTINGS.VALUE_MODE {auto} \
    CONFIG.PROT0_LR5_SETTINGS.VALUE_MODE {auto} \
    CONFIG.PROT0_LR6_SETTINGS.VALUE_MODE {auto} \
    CONFIG.PROT0_LR7_SETTINGS.VALUE_MODE {auto} \
    CONFIG.PROT0_LR8_SETTINGS.VALUE_MODE {auto} \
    CONFIG.PROT0_LR9_SETTINGS.VALUE_MODE {auto} \
    CONFIG.PROT0_NO_OF_LANES.VALUE_MODE {auto} \
    CONFIG.PROT0_RX_MASTERCLK_SRC.VALUE_MODE {auto} \
    CONFIG.PROT0_TX_MASTERCLK_SRC.VALUE_MODE {auto} \
    CONFIG.QUAD_USAGE.VALUE_MODE {auto} \
    CONFIG.REG_CONF_INTF.VALUE_MODE {auto} \
    CONFIG.RX0_LANE_SEL.VALUE_MODE {auto} \
    CONFIG.RX1_LANE_SEL.VALUE_MODE {auto} \
    CONFIG.RX2_LANE_SEL.VALUE_MODE {auto} \
    CONFIG.RX3_LANE_SEL.VALUE_MODE {auto} \
    CONFIG.TX0_LANE_SEL.VALUE_MODE {auto} \
    CONFIG.TX1_LANE_SEL.VALUE_MODE {auto} \
    CONFIG.TX2_LANE_SEL.VALUE_MODE {auto} \
    CONFIG.TX3_LANE_SEL.VALUE_MODE {auto} \
  ] $gt_quad_base_0


  # Create instance: bufg_gt_0, and set properties
  set bufg_gt_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:bufg_gt:1.0 bufg_gt_0 ]

  # Create instance: bufg_gt_1, and set properties
  set bufg_gt_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:bufg_gt:1.0 bufg_gt_1 ]

  # Create instance: clk_wizard_0, and set properties
  set clk_wizard_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wizard:1.0 clk_wizard_0 ]
  set_property -dict [list \
    CONFIG.CLKOUT_DRIVES {BUFG,BUFG,BUFG,BUFG,BUFG,BUFG,BUFG} \
    CONFIG.CLKOUT_DYN_PS {None,None,None,None,None,None,None} \
    CONFIG.CLKOUT_GROUPING {Auto,Auto,Auto,Auto,Auto,Auto,Auto} \
    CONFIG.CLKOUT_MATCHED_ROUTING {false,false,false,false,false,false,false} \
    CONFIG.CLKOUT_PORT {clk_out1,clk_out2,clk_out3,clk_out4,clk_out5,clk_out6,clk_out7} \
    CONFIG.CLKOUT_REQUESTED_DUTY_CYCLE {50.000,50.000,50.000,50.000,50.000,50.000,50.000} \
    CONFIG.CLKOUT_REQUESTED_OUT_FREQUENCY {200.000,100.000,100.000,100.000,100.000,100.000,100.000} \
    CONFIG.CLKOUT_REQUESTED_PHASE {0.000,0.000,0.000,0.000,0.000,0.000,0.000} \
    CONFIG.CLKOUT_USED {true,false,false,false,false,false,false} \
    CONFIG.PRIM_IN_FREQ {200} \
    CONFIG.PRIM_SOURCE {Differential_clock_capable_pin} \
  ] $clk_wizard_0


  # Create instance: util_ds_buf_0, and set properties
  set util_ds_buf_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_ds_buf:2.2 util_ds_buf_0 ]
  set_property CONFIG.C_BUF_TYPE {IBUFDSGTE} $util_ds_buf_0


  # Create instance: xlconcat_0, and set properties
  set xlconcat_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_0 ]

  # Create instance: xlconcat_1, and set properties
  set xlconcat_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_1 ]

  # Create instance: xlconcat_2, and set properties
  set xlconcat_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_2 ]
  set_property CONFIG.NUM_PORTS {4} $xlconcat_2


  # Create instance: util_reduced_logic_0, and set properties
  set util_reduced_logic_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_reduced_logic:2.0 util_reduced_logic_0 ]
  set_property CONFIG.C_SIZE {2} $util_reduced_logic_0


  # Create instance: util_reduced_logic_1, and set properties
  set util_reduced_logic_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_reduced_logic:2.0 util_reduced_logic_1 ]
  set_property CONFIG.C_SIZE {4} $util_reduced_logic_1


  # Create instance: util_reduced_logic_2, and set properties
  set util_reduced_logic_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_reduced_logic:2.0 util_reduced_logic_2 ]
  set_property CONFIG.C_SIZE {2} $util_reduced_logic_2


  # Create instance: xlconcat_3, and set properties
  set xlconcat_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_3 ]
  set_property CONFIG.NUM_PORTS {4} $xlconcat_3


  # Create instance: emmetcore_0, and set properties
  set block_name emmetcore
  set block_cell_name emmetcore_0
  if { [catch {set emmetcore_0 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $emmetcore_0 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create instance: axis_vio_0, and set properties
  set axis_vio_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_vio:1.0 axis_vio_0 ]
  set_property -dict [list \
    CONFIG.C_NUM_PROBE_IN {0} \
    CONFIG.C_NUM_PROBE_OUT {1} \
    CONFIG.C_PROBE_OUT1_WIDTH {1} \
    CONFIG.C_PROBE_OUT2_WIDTH {1} \
  ] $axis_vio_0


  # Create instance: emb_fifo_gen_0, and set properties
  set emb_fifo_gen_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:emb_fifo_gen:1.0 emb_fifo_gen_0 ]
  set_property -dict [list \
    CONFIG.CDC_SYNC_STAGES {2} \
    CONFIG.CLOCK_DOMAIN {Independent_Clock} \
    CONFIG.PROG_EMPTY_THRESH {5} \
    CONFIG.PROG_FULL_THRESH {2043} \
    CONFIG.RD_DATA_COUNT_WIDTH {12} \
    CONFIG.READ_MODE {FWFT} \
    CONFIG.WRITE_DATA_WIDTH {64} \
    CONFIG.WR_DATA_COUNT_WIDTH {12} \
  ] $emb_fifo_gen_0


  # Create instance: util_vector_logic_1, and set properties
  set util_vector_logic_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic:2.0 util_vector_logic_1 ]
  set_property -dict [list \
    CONFIG.C_OPERATION {not} \
    CONFIG.C_SIZE {1} \
  ] $util_vector_logic_1


  # Create instance: root_hub_core_0, and set properties
  set root_hub_core_0 [ create_bd_cell -type ip -vlnv user.org:user:root_hub_core:1.0 root_hub_core_0 ]
  set_property CONFIG.NUM_FPGAS {5} $root_hub_core_0


  # Create instance: axis_ila_1, and set properties
  set axis_ila_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_ila:1.2 axis_ila_1 ]
  set_property -dict [list \
    CONFIG.C_NUM_OF_PROBES {26} \
    CONFIG.C_PROBE0_WIDTH {64} \
    CONFIG.C_PROBE16_WIDTH {4} \
    CONFIG.C_PROBE17_WIDTH {64} \
    CONFIG.C_PROBE1_WIDTH {64} \
    CONFIG.C_PROBE3_WIDTH {64} \
    CONFIG.C_PROBE6_WIDTH {64} \
  ] $axis_ila_1


  # Create instance: emb_fifo_gen_1, and set properties
  set emb_fifo_gen_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:emb_fifo_gen:1.0 emb_fifo_gen_1 ]
  set_property -dict [list \
    CONFIG.CLOCK_DOMAIN {Independent_Clock} \
    CONFIG.PROG_EMPTY_THRESH {5} \
    CONFIG.PROG_FULL_THRESH {2043} \
    CONFIG.RD_DATA_COUNT_WIDTH {12} \
    CONFIG.READ_MODE {FWFT} \
    CONFIG.WRITE_DATA_WIDTH {64} \
    CONFIG.WR_DATA_COUNT_WIDTH {12} \
  ] $emb_fifo_gen_1


  # Create instance: util_vector_logic_0, and set properties
  set util_vector_logic_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic:2.0 util_vector_logic_0 ]
  set_property -dict [list \
    CONFIG.C_OPERATION {not} \
    CONFIG.C_SIZE {1} \
  ] $util_vector_logic_0


  # Create instance: emmetcore_1, and set properties
  set block_name emmetcore
  set block_cell_name emmetcore_1
  if { [catch {set emmetcore_1 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $emmetcore_1 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create instance: emb_fifo_gen_2, and set properties
  set emb_fifo_gen_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:emb_fifo_gen:1.0 emb_fifo_gen_2 ]
  set_property -dict [list \
    CONFIG.CLOCK_DOMAIN {Independent_Clock} \
    CONFIG.PROG_EMPTY_THRESH {5} \
    CONFIG.PROG_FULL_THRESH {2043} \
    CONFIG.RD_DATA_COUNT_WIDTH {12} \
    CONFIG.READ_MODE {FWFT} \
    CONFIG.WRITE_DATA_WIDTH {64} \
    CONFIG.WR_DATA_COUNT_WIDTH {12} \
  ] $emb_fifo_gen_2


  # Create instance: emb_fifo_gen_3, and set properties
  set emb_fifo_gen_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:emb_fifo_gen:1.0 emb_fifo_gen_3 ]
  set_property -dict [list \
    CONFIG.CLOCK_DOMAIN {Independent_Clock} \
    CONFIG.PROG_EMPTY_THRESH {5} \
    CONFIG.PROG_FULL_THRESH {2043} \
    CONFIG.RD_DATA_COUNT_WIDTH {12} \
    CONFIG.READ_MODE {FWFT} \
    CONFIG.WRITE_DATA_WIDTH {64} \
    CONFIG.WR_DATA_COUNT_WIDTH {12} \
  ] $emb_fifo_gen_3


  # Create instance: util_vector_logic_2, and set properties
  set util_vector_logic_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic:2.0 util_vector_logic_2 ]
  set_property -dict [list \
    CONFIG.C_OPERATION {not} \
    CONFIG.C_SIZE {1} \
  ] $util_vector_logic_2


  # Create instance: emmetcore_2, and set properties
  set block_name emmetcore
  set block_cell_name emmetcore_2
  if { [catch {set emmetcore_2 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $emmetcore_2 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create instance: emb_fifo_gen_4, and set properties
  set emb_fifo_gen_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:emb_fifo_gen:1.0 emb_fifo_gen_4 ]
  set_property -dict [list \
    CONFIG.CLOCK_DOMAIN {Independent_Clock} \
    CONFIG.PROG_EMPTY_THRESH {5} \
    CONFIG.PROG_FULL_THRESH {2043} \
    CONFIG.RD_DATA_COUNT_WIDTH {12} \
    CONFIG.READ_MODE {FWFT} \
    CONFIG.WRITE_DATA_WIDTH {64} \
    CONFIG.WR_DATA_COUNT_WIDTH {12} \
  ] $emb_fifo_gen_4


  # Create instance: emb_fifo_gen_5, and set properties
  set emb_fifo_gen_5 [ create_bd_cell -type ip -vlnv xilinx.com:ip:emb_fifo_gen:1.0 emb_fifo_gen_5 ]
  set_property -dict [list \
    CONFIG.CLOCK_DOMAIN {Independent_Clock} \
    CONFIG.PROG_EMPTY_THRESH {5} \
    CONFIG.PROG_FULL_THRESH {2043} \
    CONFIG.RD_DATA_COUNT_WIDTH {12} \
    CONFIG.READ_MODE {FWFT} \
    CONFIG.WRITE_DATA_WIDTH {64} \
    CONFIG.WR_DATA_COUNT_WIDTH {12} \
  ] $emb_fifo_gen_5


  # Create instance: util_vector_logic_3, and set properties
  set util_vector_logic_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic:2.0 util_vector_logic_3 ]
  set_property -dict [list \
    CONFIG.C_OPERATION {not} \
    CONFIG.C_SIZE {1} \
  ] $util_vector_logic_3


  # Create instance: emmetcore_3, and set properties
  set block_name emmetcore
  set block_cell_name emmetcore_3
  if { [catch {set emmetcore_3 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $emmetcore_3 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create instance: emb_fifo_gen_6, and set properties
  set emb_fifo_gen_6 [ create_bd_cell -type ip -vlnv xilinx.com:ip:emb_fifo_gen:1.0 emb_fifo_gen_6 ]
  set_property -dict [list \
    CONFIG.CLOCK_DOMAIN {Independent_Clock} \
    CONFIG.PROG_EMPTY_THRESH {5} \
    CONFIG.PROG_FULL_THRESH {2043} \
    CONFIG.RD_DATA_COUNT_WIDTH {12} \
    CONFIG.READ_MODE {FWFT} \
    CONFIG.WRITE_DATA_WIDTH {64} \
    CONFIG.WR_DATA_COUNT_WIDTH {12} \
  ] $emb_fifo_gen_6


  # Create instance: emb_fifo_gen_7, and set properties
  set emb_fifo_gen_7 [ create_bd_cell -type ip -vlnv xilinx.com:ip:emb_fifo_gen:1.0 emb_fifo_gen_7 ]
  set_property -dict [list \
    CONFIG.CLOCK_DOMAIN {Independent_Clock} \
    CONFIG.PROG_EMPTY_THRESH {5} \
    CONFIG.PROG_FULL_THRESH {2043} \
    CONFIG.RD_DATA_COUNT_WIDTH {12} \
    CONFIG.READ_MODE {FWFT} \
    CONFIG.WRITE_DATA_WIDTH {64} \
    CONFIG.WR_DATA_COUNT_WIDTH {12} \
  ] $emb_fifo_gen_7


  # Create instance: util_vector_logic_4, and set properties
  set util_vector_logic_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic:2.0 util_vector_logic_4 ]
  set_property -dict [list \
    CONFIG.C_OPERATION {not} \
    CONFIG.C_SIZE {1} \
  ] $util_vector_logic_4


  # Create instance: clk_wizard_1, and set properties
  set clk_wizard_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wizard:1.0 clk_wizard_1 ]
  set_property -dict [list \
    CONFIG.CLKOUT_DRIVES {BUFG,BUFG,BUFG,BUFG,BUFG,BUFG,BUFG} \
    CONFIG.CLKOUT_DYN_PS {None,None,None,None,None,None,None} \
    CONFIG.CLKOUT_GROUPING {Auto,Auto,Auto,Auto,Auto,Auto,Auto} \
    CONFIG.CLKOUT_MATCHED_ROUTING {false,false,false,false,false,false,false} \
    CONFIG.CLKOUT_PORT {clk_out1,clk_out2,clk_out3,clk_out4,clk_out5,clk_out6,clk_out7} \
    CONFIG.CLKOUT_REQUESTED_DUTY_CYCLE {50.000,50.000,50.000,50.000,50.000,50.000,50.000} \
    CONFIG.CLKOUT_REQUESTED_OUT_FREQUENCY {50.000,200.000,100.000,100.000,100.000,100.000,100.000} \
    CONFIG.CLKOUT_REQUESTED_PHASE {0.000,0.000,0.000,0.000,0.000,0.000,0.000} \
    CONFIG.CLKOUT_USED {true,false,false,false,false,false,false} \
    CONFIG.CLK_IN1_BOARD_INTERFACE {ddr4_dimm1_sma_clk} \
    CONFIG.USE_INCLK_SWITCHOVER {false} \
  ] $clk_wizard_1


  # Create instance: versal_cips_0, and set properties
  set versal_cips_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:versal_cips:3.4 versal_cips_0 ]
  set_property -dict [list \
    CONFIG.BOOT_MODE {Custom} \
    CONFIG.CLOCK_MODE {Custom} \
    CONFIG.DDR_MEMORY_MODE {Enable} \
    CONFIG.DEBUG_MODE {JTAG} \
    CONFIG.DESIGN_MODE {1} \
    CONFIG.PS_BOARD_INTERFACE {ps_pmc_fixed_io} \
    CONFIG.PS_PMC_CONFIG { \
      BOOT_MODE {Custom} \
      CLOCK_MODE {Custom} \
      DDR_MEMORY_MODE {Connectivity to DDR via NOC} \
      DEBUG_MODE {JTAG} \
      DESIGN_MODE {1} \
      PMC_ALT_REF_CLK_FREQMHZ {33.333} \
      PMC_CRP_EFUSE_REF_CTRL_SRCSEL {IRO_CLK/4} \
      PMC_CRP_HSM0_REF_CTRL_FREQMHZ {33.333} \
      PMC_CRP_HSM1_REF_CTRL_FREQMHZ {200} \
      PMC_CRP_LSBUS_REF_CTRL_FREQMHZ {100} \
      PMC_CRP_NOC_REF_CTRL_FREQMHZ {960} \
      PMC_CRP_PL0_REF_CTRL_FREQMHZ {100} \
      PMC_CRP_PL5_REF_CTRL_FREQMHZ {400} \
      PMC_GPIO0_MIO_PERIPHERAL {{ENABLE 1} {IO {PMC_MIO 0 .. 25}}} \
      PMC_GPIO1_MIO_PERIPHERAL {{ENABLE 1} {IO {PMC_MIO 26 .. 51}}} \
      PMC_HSM1_CLK_OUT_ENABLE {1} \
      PMC_MIO37 {{AUX_IO 0} {DIRECTION out} {DRIVE_STRENGTH 8mA} {OUTPUT_DATA high} {PULL pullup} {SCHMITT 0} {SLEW slow} {USAGE GPIO}} \
      PMC_OSPI_PERIPHERAL {{ENABLE 0} {IO {PMC_MIO 0 .. 11}} {MODE Single}} \
      PMC_PL_ALT_REF_CLK_FREQMHZ {33.333} \
      PMC_QSPI_COHERENCY {0} \
      PMC_QSPI_FBCLK {{ENABLE 1} {IO {PMC_MIO 6}}} \
      PMC_QSPI_PERIPHERAL_DATA_MODE {x4} \
      PMC_QSPI_PERIPHERAL_ENABLE {1} \
      PMC_QSPI_PERIPHERAL_MODE {Dual Parallel} \
      PMC_REF_CLK_FREQMHZ {33.3333} \
      PMC_SD0 {{CD_ENABLE 0} {CD_IO {PMC_MIO 24}} {POW_ENABLE 0} {POW_IO {PMC_MIO 17}} {RESET_ENABLE 0} {RESET_IO {PMC_MIO 17}} {WP_ENABLE 0} {WP_IO {PMC_MIO 25}}} \
      PMC_SD0_PERIPHERAL {{CLK_100_SDR_OTAP_DLY 0x00} {CLK_200_SDR_OTAP_DLY 0x00} {CLK_50_DDR_ITAP_DLY 0x00} {CLK_50_DDR_OTAP_DLY 0x00} {CLK_50_SDR_ITAP_DLY 0x00} {CLK_50_SDR_OTAP_DLY 0x00} {ENABLE 0}\
{IO {PMC_MIO 13 .. 25}}} \
      PMC_SD0_SLOT_TYPE {SD 2.0} \
      PMC_SD1 {{CD_ENABLE 1} {CD_IO {PMC_MIO 28}} {POW_ENABLE 1} {POW_IO {PMC_MIO 51}} {RESET_ENABLE 0} {RESET_IO {PMC_MIO 12}} {WP_ENABLE 0} {WP_IO {PMC_MIO 1}}} \
      PMC_SD1_COHERENCY {0} \
      PMC_SD1_DATA_TRANSFER_MODE {8Bit} \
      PMC_SD1_PERIPHERAL {{CLK_100_SDR_OTAP_DLY 0x3} {CLK_200_SDR_OTAP_DLY 0x2} {CLK_50_DDR_ITAP_DLY 0x36} {CLK_50_DDR_OTAP_DLY 0x3} {CLK_50_SDR_ITAP_DLY 0x2C} {CLK_50_SDR_OTAP_DLY 0x4} {ENABLE 1} {IO\
{PMC_MIO 26 .. 36}}} \
      PMC_SD1_SLOT_TYPE {SD 3.0} \
      PMC_SMAP_PERIPHERAL {{ENABLE 0} {IO {32 Bit}}} \
      PMC_USE_PMC_NOC_AXI0 {1} \
      PS_BOARD_INTERFACE {ps_pmc_fixed_io} \
      PS_CAN1_PERIPHERAL {{ENABLE 1} {IO {PMC_MIO 40 .. 41}}} \
      PS_ENET0_MDIO {{ENABLE 1} {IO {PS_MIO 24 .. 25}}} \
      PS_ENET0_PERIPHERAL {{ENABLE 1} {IO {PS_MIO 0 .. 11}}} \
      PS_ENET1_PERIPHERAL {{ENABLE 1} {IO {PS_MIO 12 .. 23}}} \
      PS_GEN_IPI0_ENABLE {1} \
      PS_GEN_IPI0_MASTER {A72} \
      PS_GEN_IPI1_ENABLE {1} \
      PS_GEN_IPI2_ENABLE {1} \
      PS_GEN_IPI3_ENABLE {1} \
      PS_GEN_IPI4_ENABLE {1} \
      PS_GEN_IPI5_ENABLE {1} \
      PS_GEN_IPI6_ENABLE {1} \
      PS_HSDP_EGRESS_TRAFFIC {JTAG} \
      PS_HSDP_INGRESS_TRAFFIC {JTAG} \
      PS_HSDP_MODE {NONE} \
      PS_I2C0_PERIPHERAL {{ENABLE 1} {IO {PMC_MIO 46 .. 47}}} \
      PS_I2C1_PERIPHERAL {{ENABLE 1} {IO {PMC_MIO 44 .. 45}}} \
      PS_MIO19 {{AUX_IO 0} {DIRECTION in} {DRIVE_STRENGTH 8mA} {OUTPUT_DATA default} {PULL disable} {SCHMITT 0} {SLEW slow} {USAGE Reserved}} \
      PS_MIO21 {{AUX_IO 0} {DIRECTION in} {DRIVE_STRENGTH 8mA} {OUTPUT_DATA default} {PULL disable} {SCHMITT 0} {SLEW slow} {USAGE Reserved}} \
      PS_MIO7 {{AUX_IO 0} {DIRECTION in} {DRIVE_STRENGTH 8mA} {OUTPUT_DATA default} {PULL disable} {SCHMITT 0} {SLEW slow} {USAGE Reserved}} \
      PS_MIO9 {{AUX_IO 0} {DIRECTION in} {DRIVE_STRENGTH 8mA} {OUTPUT_DATA default} {PULL disable} {SCHMITT 0} {SLEW slow} {USAGE Reserved}} \
      PS_NUM_FABRIC_RESETS {1} \
      PS_PCIE_EP_RESET1_IO {PMC_MIO 38} \
      PS_PCIE_EP_RESET2_IO {PMC_MIO 39} \
      PS_PCIE_RESET {ENABLE 1} \
      PS_UART0_PERIPHERAL {{ENABLE 1} {IO {PMC_MIO 42 .. 43}}} \
      PS_USB3_PERIPHERAL {{ENABLE 1} {IO {PMC_MIO 13 .. 25}}} \
      PS_USE_FPD_CCI_NOC {1} \
      PS_USE_FPD_CCI_NOC0 {1} \
      PS_USE_NOC_LPD_AXI0 {1} \
      PS_USE_PMCPL_CLK0 {1} \
      PS_USE_PMCPL_CLK1 {0} \
      PS_USE_PMCPL_CLK2 {0} \
      PS_USE_PMCPL_CLK3 {0} \
      PS_USE_PMCPL_IRO_CLK {1} \
      SMON_ALARMS {Set_Alarms_On} \
      SMON_ENABLE_TEMP_AVERAGING {0} \
      SMON_TEMP_AVERAGING_SAMPLES {0} \
    } \
    CONFIG.PS_PMC_CONFIG_APPLIED {1} \
  ] $versal_cips_0


  # Create instance: axi_noc_0, and set properties
  set axi_noc_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_noc:1.1 axi_noc_0 ]
  set_property -dict [list \
    CONFIG.CH0_DDR4_0_BOARD_INTERFACE {ddr4_dimm1} \
    CONFIG.MC_CHAN_REGION1 {DDR_LOW1} \
    CONFIG.MC_SYSTEM_CLOCK {Internal} \
    CONFIG.NUM_CLKS {7} \
    CONFIG.NUM_MC {1} \
    CONFIG.NUM_MCP {4} \
    CONFIG.NUM_MI {3} \
    CONFIG.NUM_SI {6} \
    CONFIG.sys_clk0_BOARD_INTERFACE {ddr4_dimm1_sma_clk} \
  ] $axi_noc_0


  set_property -dict [ list \
   CONFIG.APERTURES {{0x201_0000_0000 1G}} \
   CONFIG.CATEGORY {pl} \
 ] [get_bd_intf_pins /axi_noc_0/M00_AXI]

  set_property -dict [ list \
   CONFIG.APERTURES {{0x201_8000_0000 1G}} \
   CONFIG.CATEGORY {pl} \
 ] [get_bd_intf_pins /axi_noc_0/M01_AXI]

  set_property -dict [ list \
   CONFIG.APERTURES {{0x202_0000_0000 1G}} \
   CONFIG.CATEGORY {pl} \
 ] [get_bd_intf_pins /axi_noc_0/M02_AXI]

  set_property -dict [ list \
   CONFIG.REGION {0} \
   CONFIG.CONNECTIONS {M01_AXI {read_bw {5} write_bw {5}} M02_AXI {read_bw {5} write_bw {5}} M00_AXI {read_bw {5} write_bw {5}} MC_3 {read_bw {100} write_bw {100} read_avg_burst {4} write_avg_burst {4}}} \
   CONFIG.DEST_IDS {M01_AXI:0x0:M02_AXI:0x80:M00_AXI:0x40} \
   CONFIG.NOC_PARAMS {} \
   CONFIG.CATEGORY {ps_cci} \
 ] [get_bd_intf_pins /axi_noc_0/S00_AXI]

  set_property -dict [ list \
   CONFIG.REGION {0} \
   CONFIG.CONNECTIONS {M01_AXI {read_bw {500} write_bw {500} read_avg_burst {4} write_avg_burst {4}} M02_AXI {read_bw {500} write_bw {500} read_avg_burst {4} write_avg_burst {4}} M00_AXI {read_bw {500} write_bw {500} read_avg_burst {4} write_avg_burst {4}} MC_2 {read_bw {100} write_bw {100} read_avg_burst {4} write_avg_burst {4}}} \
   CONFIG.DEST_IDS {M01_AXI:0x0:M02_AXI:0x80:M00_AXI:0x40} \
   CONFIG.NOC_PARAMS {} \
   CONFIG.CATEGORY {ps_cci} \
 ] [get_bd_intf_pins /axi_noc_0/S01_AXI]

  set_property -dict [ list \
   CONFIG.REGION {0} \
   CONFIG.CONNECTIONS {MC_0 {read_bw {100} write_bw {100} read_avg_burst {4} write_avg_burst {4}} M01_AXI {read_bw {500} write_bw {500} read_avg_burst {4} write_avg_burst {4}} M02_AXI {read_bw {500} write_bw {500} read_avg_burst {4} write_avg_burst {4}} M00_AXI {read_bw {500} write_bw {500} read_avg_burst {4} write_avg_burst {4}}} \
   CONFIG.DEST_IDS {M01_AXI:0x0:M02_AXI:0x80:M00_AXI:0x40} \
   CONFIG.NOC_PARAMS {} \
   CONFIG.CATEGORY {ps_cci} \
 ] [get_bd_intf_pins /axi_noc_0/S02_AXI]

  set_property -dict [ list \
   CONFIG.REGION {0} \
   CONFIG.CONNECTIONS {M01_AXI {read_bw {500} write_bw {500} read_avg_burst {4} write_avg_burst {4}} M02_AXI {read_bw {500} write_bw {500} read_avg_burst {4} write_avg_burst {4}} M00_AXI {read_bw {500} write_bw {500} read_avg_burst {4} write_avg_burst {4}} MC_1 {read_bw {100} write_bw {100} read_avg_burst {4} write_avg_burst {4}}} \
   CONFIG.DEST_IDS {M01_AXI:0x0:M02_AXI:0x80:M00_AXI:0x40} \
   CONFIG.NOC_PARAMS {} \
   CONFIG.CATEGORY {ps_cci} \
 ] [get_bd_intf_pins /axi_noc_0/S03_AXI]

  set_property -dict [ list \
   CONFIG.REGION {0} \
   CONFIG.CONNECTIONS {M01_AXI {read_bw {500} write_bw {500} read_avg_burst {4} write_avg_burst {4}} M02_AXI {read_bw {500} write_bw {500} read_avg_burst {4} write_avg_burst {4}} M00_AXI {read_bw {500} write_bw {500} read_avg_burst {4} write_avg_burst {4}} MC_3 {read_bw {100} write_bw {100} read_avg_burst {4} write_avg_burst {4}}} \
   CONFIG.DEST_IDS {M01_AXI:0x0:M02_AXI:0x80:M00_AXI:0x40} \
   CONFIG.NOC_PARAMS {} \
   CONFIG.CATEGORY {ps_rpu} \
 ] [get_bd_intf_pins /axi_noc_0/S04_AXI]

  set_property -dict [ list \
   CONFIG.REGION {0} \
   CONFIG.CONNECTIONS {M01_AXI {read_bw {500} write_bw {500} read_avg_burst {4} write_avg_burst {4}} M02_AXI {read_bw {500} write_bw {500} read_avg_burst {4} write_avg_burst {4}} M00_AXI {read_bw {500} write_bw {500} read_avg_burst {4} write_avg_burst {4}} MC_2 {read_bw {100} write_bw {100} read_avg_burst {4} write_avg_burst {4}}} \
   CONFIG.DEST_IDS {M01_AXI:0x0:M02_AXI:0x80:M00_AXI:0x40} \
   CONFIG.NOC_PARAMS {} \
   CONFIG.CATEGORY {ps_pmc} \
 ] [get_bd_intf_pins /axi_noc_0/S05_AXI]

  set_property -dict [ list \
   CONFIG.ASSOCIATED_BUSIF {S00_AXI} \
 ] [get_bd_pins /axi_noc_0/aclk0]

  set_property -dict [ list \
   CONFIG.ASSOCIATED_BUSIF {S01_AXI} \
 ] [get_bd_pins /axi_noc_0/aclk1]

  set_property -dict [ list \
   CONFIG.ASSOCIATED_BUSIF {S02_AXI} \
 ] [get_bd_pins /axi_noc_0/aclk2]

  set_property -dict [ list \
   CONFIG.ASSOCIATED_BUSIF {S03_AXI} \
 ] [get_bd_pins /axi_noc_0/aclk3]

  set_property -dict [ list \
   CONFIG.ASSOCIATED_BUSIF {S04_AXI} \
 ] [get_bd_pins /axi_noc_0/aclk4]

  set_property -dict [ list \
   CONFIG.ASSOCIATED_BUSIF {S05_AXI} \
 ] [get_bd_pins /axi_noc_0/aclk5]

  set_property -dict [ list \
   CONFIG.ASSOCIATED_BUSIF {M00_AXI:M01_AXI:M02_AXI} \
 ] [get_bd_pins /axi_noc_0/aclk6]

  # Create instance: send_bram, and set properties
  set send_bram [ create_bd_cell -type ip -vlnv xilinx.com:ip:emb_mem_gen:1.0 send_bram ]
  set_property -dict [list \
    CONFIG.ENABLE_BYTE_WRITES_A {true} \
    CONFIG.MEMORY_TYPE {True_Dual_Port_RAM} \
    CONFIG.READ_LATENCY_A {1} \
    CONFIG.READ_LATENCY_B {1} \
    CONFIG.USE_MEMORY_BLOCK {Stand_Alone} \
    CONFIG.WRITE_DATA_WIDTH_A {64} \
    CONFIG.WRITE_MODE_A {WRITE_FIRST} \
    CONFIG.WRITE_MODE_B {WRITE_FIRST} \
  ] $send_bram


  # Create instance: axi_bram_ctrl_send, and set properties
  set axi_bram_ctrl_send [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl_send ]
  set_property -dict [list \
    CONFIG.DATA_WIDTH {64} \
    CONFIG.SINGLE_PORT_BRAM {1} \
  ] $axi_bram_ctrl_send


  # Create instance: rst_versal_cips_0_333M, and set properties
  set rst_versal_cips_0_333M [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 rst_versal_cips_0_333M ]

  # Create instance: axi_bram_ctrl_status, and set properties
  set axi_bram_ctrl_status [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl_status ]
  set_property -dict [list \
    CONFIG.DATA_WIDTH {64} \
    CONFIG.SINGLE_PORT_BRAM {1} \
  ] $axi_bram_ctrl_status


  # Create instance: status_bram, and set properties
  set status_bram [ create_bd_cell -type ip -vlnv xilinx.com:ip:emb_mem_gen:1.0 status_bram ]
  set_property -dict [list \
    CONFIG.ENABLE_BYTE_WRITES_A {true} \
    CONFIG.MEMORY_TYPE {True_Dual_Port_RAM} \
    CONFIG.READ_LATENCY_A {1} \
    CONFIG.READ_LATENCY_B {1} \
    CONFIG.USE_MEMORY_BLOCK {Stand_Alone} \
    CONFIG.WRITE_DATA_WIDTH_A {64} \
    CONFIG.WRITE_MODE_A {WRITE_FIRST} \
    CONFIG.WRITE_MODE_B {WRITE_FIRST} \
  ] $status_bram


  # Create instance: axi_bram_ctrl_rec, and set properties
  set axi_bram_ctrl_rec [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl_rec ]
  set_property -dict [list \
    CONFIG.DATA_WIDTH {64} \
    CONFIG.SINGLE_PORT_BRAM {1} \
  ] $axi_bram_ctrl_rec


  # Create instance: rec_bram, and set properties
  set rec_bram [ create_bd_cell -type ip -vlnv xilinx.com:ip:emb_mem_gen:1.0 rec_bram ]
  set_property -dict [list \
    CONFIG.ENABLE_BYTE_WRITES_A {true} \
    CONFIG.MEMORY_TYPE {True_Dual_Port_RAM} \
    CONFIG.READ_LATENCY_A {1} \
    CONFIG.READ_LATENCY_B {1} \
    CONFIG.USE_MEMORY_BLOCK {Stand_Alone} \
    CONFIG.WRITE_DATA_WIDTH_A {64} \
    CONFIG.WRITE_MODE_A {WRITE_FIRST} \
    CONFIG.WRITE_MODE_B {WRITE_FIRST} \
  ] $rec_bram


  # Create instance: FSM_Send_to_FPGA, and set properties
  set block_name FSM_Send_Root
  set block_cell_name FSM_Send_to_FPGA
  if { [catch {set FSM_Send_to_FPGA [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $FSM_Send_to_FPGA eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create instance: xlconstant_0, and set properties
  set xlconstant_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_0 ]

  # Create instance: CPU_to_FPGA, and set properties
  set CPU_to_FPGA [ create_bd_cell -type ip -vlnv xilinx.com:ip:emb_fifo_gen:1.0 CPU_to_FPGA ]
  set_property -dict [list \
    CONFIG.READ_MODE {FWFT} \
    CONFIG.WRITE_DATA_WIDTH {64} \
  ] $CPU_to_FPGA


  # Create instance: FSM_Rec_from_FPGA, and set properties
  set block_name FSM_Rec_Root
  set block_cell_name FSM_Rec_from_FPGA
  if { [catch {set FSM_Rec_from_FPGA [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $FSM_Rec_from_FPGA eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create instance: FPGA_to_CPU, and set properties
  set FPGA_to_CPU [ create_bd_cell -type ip -vlnv xilinx.com:ip:emb_fifo_gen:1.0 FPGA_to_CPU ]
  set_property -dict [list \
    CONFIG.FIFO_WRITE_DEPTH {256} \
    CONFIG.READ_MODE {FWFT} \
    CONFIG.WRITE_DATA_WIDTH {64} \
  ] $FPGA_to_CPU


  # Create instance: FSM_Mul_Root_0, and set properties
  set block_name FSM_Mul_Root
  set block_cell_name FSM_Mul_Root_0
  if { [catch {set FSM_Mul_Root_0 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $FSM_Mul_Root_0 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create interface connections
  connect_bd_intf_net -intf_net CLK_IN1_D_0_1 [get_bd_intf_ports CLK_IN1_D_0] [get_bd_intf_pins clk_wizard_0/CLK_IN1_D]
  connect_bd_intf_net -intf_net CLK_IN_D_0_1 [get_bd_intf_ports CLK_IN_D_0] [get_bd_intf_pins util_ds_buf_0/CLK_IN_D]
  connect_bd_intf_net -intf_net PS_PL_communication_ddr4_dimm1 [get_bd_intf_ports ddr4_dimm1] [get_bd_intf_pins axi_noc_0/CH0_DDR4_0]
  connect_bd_intf_net -intf_net axi_bram_ctrl_0_BRAM_PORTA [get_bd_intf_pins send_bram/BRAM_PORTA] [get_bd_intf_pins axi_bram_ctrl_send/BRAM_PORTA]
  connect_bd_intf_net -intf_net axi_bram_ctrl_1_BRAM_PORTA [get_bd_intf_pins axi_bram_ctrl_status/BRAM_PORTA] [get_bd_intf_pins status_bram/BRAM_PORTA]
  connect_bd_intf_net -intf_net axi_bram_ctrl_2_BRAM_PORTA [get_bd_intf_pins axi_bram_ctrl_rec/BRAM_PORTA] [get_bd_intf_pins rec_bram/BRAM_PORTA]
  connect_bd_intf_net -intf_net axi_noc_0_M00_AXI [get_bd_intf_pins axi_bram_ctrl_send/S_AXI] [get_bd_intf_pins axi_noc_0/M00_AXI]
  connect_bd_intf_net -intf_net axi_noc_0_M01_AXI [get_bd_intf_pins axi_bram_ctrl_status/S_AXI] [get_bd_intf_pins axi_noc_0/M01_AXI]
  connect_bd_intf_net -intf_net axi_noc_0_M02_AXI [get_bd_intf_pins axi_bram_ctrl_rec/S_AXI] [get_bd_intf_pins axi_noc_0/M02_AXI]
  connect_bd_intf_net -intf_net ddr4_dimm1_sma_clk_1 [get_bd_intf_ports ddr4_dimm1_sma_clk] [get_bd_intf_pins clk_wizard_1/CLK_IN1_D]
  connect_bd_intf_net -intf_net gt_bridge_ip_0_GT_RX0 [get_bd_intf_pins gt_bridge_ip_0/GT_RX0] [get_bd_intf_pins gt_quad_base_0/RX0_GT_IP_Interface]
  connect_bd_intf_net -intf_net gt_bridge_ip_0_GT_RX1 [get_bd_intf_pins gt_bridge_ip_0/GT_RX1] [get_bd_intf_pins gt_quad_base_0/RX1_GT_IP_Interface]
  connect_bd_intf_net -intf_net gt_bridge_ip_0_GT_RX2 [get_bd_intf_pins gt_bridge_ip_0/GT_RX2] [get_bd_intf_pins gt_quad_base_0/RX2_GT_IP_Interface]
  connect_bd_intf_net -intf_net gt_bridge_ip_0_GT_RX3 [get_bd_intf_pins gt_bridge_ip_0/GT_RX3] [get_bd_intf_pins gt_quad_base_0/RX3_GT_IP_Interface]
  connect_bd_intf_net -intf_net gt_bridge_ip_0_GT_TX0 [get_bd_intf_pins gt_bridge_ip_0/GT_TX0] [get_bd_intf_pins gt_quad_base_0/TX0_GT_IP_Interface]
  connect_bd_intf_net -intf_net gt_bridge_ip_0_GT_TX1 [get_bd_intf_pins gt_bridge_ip_0/GT_TX1] [get_bd_intf_pins gt_quad_base_0/TX1_GT_IP_Interface]
  connect_bd_intf_net -intf_net gt_bridge_ip_0_GT_TX2 [get_bd_intf_pins gt_bridge_ip_0/GT_TX2] [get_bd_intf_pins gt_quad_base_0/TX2_GT_IP_Interface]
  connect_bd_intf_net -intf_net gt_bridge_ip_0_GT_TX3 [get_bd_intf_pins gt_bridge_ip_0/GT_TX3] [get_bd_intf_pins gt_quad_base_0/TX3_GT_IP_Interface]
  connect_bd_intf_net -intf_net gt_quad_base_0_GT_Serial [get_bd_intf_ports GT_Serial_0] [get_bd_intf_pins gt_quad_base_0/GT_Serial]
  connect_bd_intf_net -intf_net versal_cips_0_FPD_CCI_NOC_0 [get_bd_intf_pins versal_cips_0/FPD_CCI_NOC_0] [get_bd_intf_pins axi_noc_0/S00_AXI]
  connect_bd_intf_net -intf_net versal_cips_0_FPD_CCI_NOC_1 [get_bd_intf_pins versal_cips_0/FPD_CCI_NOC_1] [get_bd_intf_pins axi_noc_0/S01_AXI]
  connect_bd_intf_net -intf_net versal_cips_0_FPD_CCI_NOC_2 [get_bd_intf_pins versal_cips_0/FPD_CCI_NOC_2] [get_bd_intf_pins axi_noc_0/S02_AXI]
  connect_bd_intf_net -intf_net versal_cips_0_FPD_CCI_NOC_3 [get_bd_intf_pins versal_cips_0/FPD_CCI_NOC_3] [get_bd_intf_pins axi_noc_0/S03_AXI]
  connect_bd_intf_net -intf_net versal_cips_0_LPD_AXI_NOC_0 [get_bd_intf_pins versal_cips_0/LPD_AXI_NOC_0] [get_bd_intf_pins axi_noc_0/S04_AXI]
  connect_bd_intf_net -intf_net versal_cips_0_PMC_NOC_AXI_0 [get_bd_intf_pins versal_cips_0/PMC_NOC_AXI_0] [get_bd_intf_pins axi_noc_0/S05_AXI]

  # Create port connections
  connect_bd_net -net CPU_to_FPGA_wr_ack [get_bd_pins CPU_to_FPGA/wr_ack] [get_bd_pins FSM_Send_to_FPGA/wr_ack_i]
  connect_bd_net -net FSM_Mul_Root_0_bram_addr_o [get_bd_pins FSM_Mul_Root_0/bram_addr_o] [get_bd_pins status_bram/addrb]
  connect_bd_net -net FSM_Mul_Root_0_bram_write_o [get_bd_pins FSM_Mul_Root_0/bram_write_o] [get_bd_pins status_bram/web]
  connect_bd_net -net FSM_Mul_Root_0_data_bram_o [get_bd_pins FSM_Mul_Root_0/data_bram_o] [get_bd_pins status_bram/dinb]
  connect_bd_net -net FSM_Rec_Root_0_bram_addr_o [get_bd_pins FSM_Rec_from_FPGA/bram_addr_o] [get_bd_pins rec_bram/addrb]
  connect_bd_net -net FSM_Rec_Root_0_bram_write_o [get_bd_pins FSM_Rec_from_FPGA/bram_write_o] [get_bd_pins rec_bram/web]
  connect_bd_net -net FSM_Rec_Root_0_data_bram_o [get_bd_pins FSM_Rec_from_FPGA/data_bram_o] [get_bd_pins rec_bram/dinb]
  connect_bd_net -net FSM_Rec_Root_0_rd_en_fifo_o [get_bd_pins FSM_Rec_from_FPGA/rd_en_fifo_o] [get_bd_pins FPGA_to_CPU/rd_en]
  connect_bd_net -net FSM_Send_Root_0_bram_addr_o [get_bd_pins FSM_Send_to_FPGA/bram_addr_o] [get_bd_pins send_bram/addrb]
  connect_bd_net -net FSM_Send_Root_0_bram_write_o [get_bd_pins FSM_Send_to_FPGA/bram_write_o] [get_bd_pins send_bram/web]
  connect_bd_net -net FSM_Send_Root_0_data_bram_o [get_bd_pins FSM_Send_to_FPGA/data_bram_o] [get_bd_pins send_bram/dinb]
  connect_bd_net -net FSM_Send_Root_0_data_to_root [get_bd_pins FSM_Send_to_FPGA/data_to_root] [get_bd_pins CPU_to_FPGA/din]
  connect_bd_net -net FSM_Send_Root_0_wr_fifo_o [get_bd_pins FSM_Send_to_FPGA/wr_fifo_o] [get_bd_pins CPU_to_FPGA/wr_en]
  connect_bd_net -net Net [get_bd_pins axi_bram_ctrl_rec/bram_clk_a] [get_bd_pins rec_bram/clkb] [get_bd_pins rec_bram/clka]
  connect_bd_net -net Net1 [get_bd_pins axi_bram_ctrl_rec/bram_rst_a] [get_bd_pins rec_bram/rstb] [get_bd_pins rec_bram/rsta]
  connect_bd_net -net PS_PL_communication_dout_0 [get_bd_pins CPU_to_FPGA/dout] [get_bd_pins axis_ila_1/probe0] [get_bd_pins root_hub_core_0/rx_0_din]
  connect_bd_net -net PS_PL_communication_empty_0 [get_bd_pins CPU_to_FPGA/empty] [get_bd_pins axis_ila_1/probe10] [get_bd_pins root_hub_core_0/rx_0_empty]
  connect_bd_net -net PS_PL_communication_full_0 [get_bd_pins FPGA_to_CPU/full] [get_bd_pins root_hub_core_0/tx_0_full]
  connect_bd_net -net axi_bram_ctrl_rec_bram_addr_a [get_bd_pins axi_bram_ctrl_rec/bram_addr_a] [get_bd_pins rec_bram/addra]
  connect_bd_net -net axi_bram_ctrl_rec_bram_en_a [get_bd_pins axi_bram_ctrl_rec/bram_en_a] [get_bd_pins rec_bram/ena]
  connect_bd_net -net axi_bram_ctrl_rec_bram_we_a [get_bd_pins axi_bram_ctrl_rec/bram_we_a] [get_bd_pins rec_bram/wea]
  connect_bd_net -net axi_bram_ctrl_rec_bram_wrdata_a [get_bd_pins axi_bram_ctrl_rec/bram_wrdata_a] [get_bd_pins rec_bram/dina]
  connect_bd_net -net axi_bram_ctrl_send_bram_addr_a [get_bd_pins axi_bram_ctrl_send/bram_addr_a] [get_bd_pins send_bram/addra]
  connect_bd_net -net axi_bram_ctrl_send_bram_clk_a [get_bd_pins axi_bram_ctrl_send/bram_clk_a] [get_bd_pins send_bram/clka] [get_bd_pins send_bram/clkb]
  connect_bd_net -net axi_bram_ctrl_send_bram_en_a [get_bd_pins axi_bram_ctrl_send/bram_en_a] [get_bd_pins send_bram/ena]
  connect_bd_net -net axi_bram_ctrl_send_bram_rst_a [get_bd_pins axi_bram_ctrl_send/bram_rst_a] [get_bd_pins send_bram/rsta] [get_bd_pins send_bram/rstb]
  connect_bd_net -net axi_bram_ctrl_send_bram_we_a [get_bd_pins axi_bram_ctrl_send/bram_we_a] [get_bd_pins send_bram/wea]
  connect_bd_net -net axi_bram_ctrl_send_bram_wrdata_a [get_bd_pins axi_bram_ctrl_send/bram_wrdata_a] [get_bd_pins send_bram/dina]
  connect_bd_net -net axi_bram_ctrl_status_bram_addr_a [get_bd_pins axi_bram_ctrl_status/bram_addr_a] [get_bd_pins status_bram/addra]
  connect_bd_net -net axi_bram_ctrl_status_bram_clk_a [get_bd_pins axi_bram_ctrl_status/bram_clk_a] [get_bd_pins status_bram/clka] [get_bd_pins status_bram/clkb]
  connect_bd_net -net axi_bram_ctrl_status_bram_en_a [get_bd_pins axi_bram_ctrl_status/bram_en_a] [get_bd_pins status_bram/ena]
  connect_bd_net -net axi_bram_ctrl_status_bram_rst_a [get_bd_pins axi_bram_ctrl_status/bram_rst_a] [get_bd_pins status_bram/rstb] [get_bd_pins status_bram/rsta]
  connect_bd_net -net axi_bram_ctrl_status_bram_we_a [get_bd_pins axi_bram_ctrl_status/bram_we_a] [get_bd_pins status_bram/wea]
  connect_bd_net -net axi_bram_ctrl_status_bram_wrdata_a [get_bd_pins axi_bram_ctrl_status/bram_wrdata_a] [get_bd_pins status_bram/dina]
  connect_bd_net -net axis_vio_0_probe_out0 [get_bd_pins axis_vio_0/probe_out0] [get_bd_pins emmetcore_0/tx_corereset_n] [get_bd_pins emmetcore_1/tx_corereset_n] [get_bd_pins emmetcore_2/tx_corereset_n] [get_bd_pins emmetcore_3/tx_corereset_n] [get_bd_pins util_vector_logic_0/Op1]
  connect_bd_net -net bufg_gt_0_usrclk [get_bd_pins bufg_gt_0/usrclk] [get_bd_pins emmetcore_0/tx_userclk] [get_bd_pins emmetcore_1/tx_userclk] [get_bd_pins emmetcore_2/tx_userclk] [get_bd_pins emmetcore_3/tx_userclk] [get_bd_pins emb_fifo_gen_0/rd_clk] [get_bd_pins emb_fifo_gen_2/rd_clk] [get_bd_pins emb_fifo_gen_4/rd_clk] [get_bd_pins emb_fifo_gen_6/rd_clk] [get_bd_pins gt_bridge_ip_0/gt_txusrclk] [get_bd_pins gt_quad_base_0/ch0_txusrclk] [get_bd_pins gt_quad_base_0/ch1_txusrclk] [get_bd_pins gt_quad_base_0/ch2_txusrclk] [get_bd_pins gt_quad_base_0/ch3_txusrclk]
  connect_bd_net -net bufg_gt_1_usrclk [get_bd_pins bufg_gt_1/usrclk] [get_bd_pins emmetcore_0/rx_userclk] [get_bd_pins emmetcore_1/rx_userclk] [get_bd_pins emmetcore_2/rx_userclk] [get_bd_pins emmetcore_3/rx_userclk] [get_bd_pins emb_fifo_gen_1/wr_clk] [get_bd_pins emb_fifo_gen_3/wr_clk] [get_bd_pins emb_fifo_gen_5/wr_clk] [get_bd_pins emb_fifo_gen_7/wr_clk] [get_bd_pins gt_bridge_ip_0/gt_rxusrclk] [get_bd_pins gt_quad_base_0/ch0_rxusrclk] [get_bd_pins gt_quad_base_0/ch1_rxusrclk] [get_bd_pins gt_quad_base_0/ch2_rxusrclk] [get_bd_pins gt_quad_base_0/ch3_rxusrclk]
  connect_bd_net -net clk_wizard_0_clk_out1 [get_bd_pins clk_wizard_0/clk_out1] [get_bd_pins emmetcore_0/init_clk] [get_bd_pins emmetcore_1/init_clk] [get_bd_pins emmetcore_2/init_clk] [get_bd_pins emmetcore_3/init_clk] [get_bd_pins gt_bridge_ip_0/apb3clk] [get_bd_pins gt_quad_base_0/altclk] [get_bd_pins gt_quad_base_0/apb3clk]
  connect_bd_net -net clk_wizard_1_clk_out1 [get_bd_pins clk_wizard_1/clk_out1] [get_bd_pins axis_ila_1/clk] [get_bd_pins axis_vio_0/clk] [get_bd_pins emb_fifo_gen_0/wr_clk] [get_bd_pins emb_fifo_gen_1/rd_clk] [get_bd_pins emb_fifo_gen_2/wr_clk] [get_bd_pins emb_fifo_gen_3/rd_clk] [get_bd_pins emb_fifo_gen_4/wr_clk] [get_bd_pins emb_fifo_gen_5/rd_clk] [get_bd_pins emb_fifo_gen_6/wr_clk] [get_bd_pins emb_fifo_gen_7/rd_clk] [get_bd_pins root_hub_core_0/clk]
  connect_bd_net -net emb_fifo_gen_0_dout [get_bd_pins emb_fifo_gen_0/dout] [get_bd_pins emmetcore_0/tx_data]
  connect_bd_net -net emb_fifo_gen_0_dout_1 [get_bd_pins FPGA_to_CPU/dout] [get_bd_pins FSM_Rec_from_FPGA/data_from_fpga_i]
  connect_bd_net -net emb_fifo_gen_0_empty [get_bd_pins emb_fifo_gen_0/empty] [get_bd_pins util_vector_logic_1/Op1]
  connect_bd_net -net emb_fifo_gen_0_empty_1 [get_bd_pins FPGA_to_CPU/empty] [get_bd_pins FSM_Rec_from_FPGA/empty_fifo_i]
  connect_bd_net -net emb_fifo_gen_0_full [get_bd_pins emb_fifo_gen_0/full] [get_bd_pins axis_ila_1/probe12] [get_bd_pins root_hub_core_0/tx_1_full]
  connect_bd_net -net emb_fifo_gen_1_dout [get_bd_pins emb_fifo_gen_1/dout] [get_bd_pins root_hub_core_0/rx_1_din]
  connect_bd_net -net emb_fifo_gen_1_empty [get_bd_pins emb_fifo_gen_1/empty] [get_bd_pins axis_ila_1/probe14] [get_bd_pins root_hub_core_0/rx_1_empty]
  connect_bd_net -net emb_fifo_gen_2_dout [get_bd_pins emb_fifo_gen_2/dout] [get_bd_pins emmetcore_1/tx_data]
  connect_bd_net -net emb_fifo_gen_2_empty [get_bd_pins emb_fifo_gen_2/empty] [get_bd_pins util_vector_logic_2/Op1] [get_bd_pins axis_ila_1/probe24]
  connect_bd_net -net emb_fifo_gen_2_full [get_bd_pins emb_fifo_gen_2/full] [get_bd_pins axis_ila_1/probe13] [get_bd_pins root_hub_core_0/tx_2_full]
  connect_bd_net -net emb_fifo_gen_3_dout [get_bd_pins emb_fifo_gen_3/dout] [get_bd_pins root_hub_core_0/rx_2_din]
  connect_bd_net -net emb_fifo_gen_3_empty [get_bd_pins emb_fifo_gen_3/empty] [get_bd_pins axis_ila_1/probe15] [get_bd_pins root_hub_core_0/rx_2_empty]
  connect_bd_net -net emb_fifo_gen_4_dout [get_bd_pins emb_fifo_gen_4/dout] [get_bd_pins emmetcore_2/tx_data]
  connect_bd_net -net emb_fifo_gen_4_empty [get_bd_pins emb_fifo_gen_4/empty] [get_bd_pins util_vector_logic_3/Op1]
  connect_bd_net -net emb_fifo_gen_4_full [get_bd_pins emb_fifo_gen_4/full] [get_bd_pins root_hub_core_0/tx_3_full]
  connect_bd_net -net emb_fifo_gen_5_dout [get_bd_pins emb_fifo_gen_5/dout] [get_bd_pins root_hub_core_0/rx_3_din]
  connect_bd_net -net emb_fifo_gen_5_empty [get_bd_pins emb_fifo_gen_5/empty] [get_bd_pins axis_ila_1/probe20] [get_bd_pins root_hub_core_0/rx_3_empty]
  connect_bd_net -net emb_fifo_gen_6_dout [get_bd_pins emb_fifo_gen_6/dout] [get_bd_pins emmetcore_3/tx_data]
  connect_bd_net -net emb_fifo_gen_6_empty [get_bd_pins emb_fifo_gen_6/empty] [get_bd_pins util_vector_logic_4/Op1]
  connect_bd_net -net emb_fifo_gen_6_full [get_bd_pins emb_fifo_gen_6/full] [get_bd_pins root_hub_core_0/tx_4_full]
  connect_bd_net -net emb_fifo_gen_7_dout [get_bd_pins emb_fifo_gen_7/dout] [get_bd_pins root_hub_core_0/rx_4_din]
  connect_bd_net -net emb_fifo_gen_7_empty [get_bd_pins emb_fifo_gen_7/empty] [get_bd_pins axis_ila_1/probe21] [get_bd_pins root_hub_core_0/rx_4_empty]
  connect_bd_net -net emmetcore_0_rx_data [get_bd_pins emmetcore_0/rx_data] [get_bd_pins emb_fifo_gen_1/din]
  connect_bd_net -net emmetcore_0_rx_gearboxslip [get_bd_pins emmetcore_0/rx_gearboxslip] [get_bd_pins gt_bridge_ip_0/ch0_rxgearboxslip_ext]
  connect_bd_net -net emmetcore_0_rx_polarity [get_bd_pins emmetcore_0/rx_polarity] [get_bd_pins gt_bridge_ip_0/ch0_rxpolarity_ext]
  connect_bd_net -net emmetcore_0_rx_valid [get_bd_pins emmetcore_0/rx_valid] [get_bd_pins emb_fifo_gen_1/wr_en]
  connect_bd_net -net emmetcore_0_tx_header_out [get_bd_pins emmetcore_0/tx_header_out] [get_bd_pins gt_bridge_ip_0/ch0_txheader_ext]
  connect_bd_net -net emmetcore_0_tx_ready [get_bd_pins emmetcore_0/tx_ready] [get_bd_pins emb_fifo_gen_0/rd_en]
  connect_bd_net -net emmetcore_0_tx_sequence_out [get_bd_pins emmetcore_0/tx_sequence_out] [get_bd_pins gt_bridge_ip_0/ch0_txsequence_ext]
  connect_bd_net -net emmetcore_0_tx_userdata_out [get_bd_pins emmetcore_0/tx_userdata_out] [get_bd_pins gt_bridge_ip_0/ch0_txdata_ext]
  connect_bd_net -net emmetcore_1_rx_data [get_bd_pins emmetcore_1/rx_data] [get_bd_pins emb_fifo_gen_3/din]
  connect_bd_net -net emmetcore_1_rx_gearboxslip [get_bd_pins emmetcore_1/rx_gearboxslip] [get_bd_pins gt_bridge_ip_0/ch1_rxgearboxslip_ext]
  connect_bd_net -net emmetcore_1_rx_polarity [get_bd_pins emmetcore_1/rx_polarity] [get_bd_pins gt_bridge_ip_0/ch1_rxpolarity_ext]
  connect_bd_net -net emmetcore_1_rx_valid [get_bd_pins emmetcore_1/rx_valid] [get_bd_pins emb_fifo_gen_3/wr_en]
  connect_bd_net -net emmetcore_1_tx_header_out [get_bd_pins emmetcore_1/tx_header_out] [get_bd_pins gt_bridge_ip_0/ch1_txheader_ext]
  connect_bd_net -net emmetcore_1_tx_ready [get_bd_pins emmetcore_1/tx_ready] [get_bd_pins emb_fifo_gen_2/rd_en] [get_bd_pins axis_ila_1/probe25]
  connect_bd_net -net emmetcore_1_tx_sequence_out [get_bd_pins emmetcore_1/tx_sequence_out] [get_bd_pins gt_bridge_ip_0/ch1_txsequence_ext]
  connect_bd_net -net emmetcore_1_tx_userdata_out [get_bd_pins emmetcore_1/tx_userdata_out] [get_bd_pins gt_bridge_ip_0/ch1_txdata_ext]
  connect_bd_net -net emmetcore_2_rx_data [get_bd_pins emmetcore_2/rx_data] [get_bd_pins emb_fifo_gen_5/din]
  connect_bd_net -net emmetcore_2_rx_gearboxslip [get_bd_pins emmetcore_2/rx_gearboxslip] [get_bd_pins gt_bridge_ip_0/ch2_rxgearboxslip_ext]
  connect_bd_net -net emmetcore_2_rx_polarity [get_bd_pins emmetcore_2/rx_polarity] [get_bd_pins gt_bridge_ip_0/ch2_rxpolarity_ext]
  connect_bd_net -net emmetcore_2_rx_valid [get_bd_pins emmetcore_2/rx_valid] [get_bd_pins emb_fifo_gen_5/wr_en]
  connect_bd_net -net emmetcore_2_tx_header_out [get_bd_pins emmetcore_2/tx_header_out] [get_bd_pins gt_bridge_ip_0/ch2_txheader_ext]
  connect_bd_net -net emmetcore_2_tx_ready [get_bd_pins emmetcore_2/tx_ready] [get_bd_pins emb_fifo_gen_4/rd_en]
  connect_bd_net -net emmetcore_2_tx_sequence_out [get_bd_pins emmetcore_2/tx_sequence_out] [get_bd_pins gt_bridge_ip_0/ch2_txsequence_ext]
  connect_bd_net -net emmetcore_2_tx_userdata_out [get_bd_pins emmetcore_2/tx_userdata_out] [get_bd_pins gt_bridge_ip_0/ch2_txdata_ext]
  connect_bd_net -net emmetcore_3_rx_data [get_bd_pins emmetcore_3/rx_data] [get_bd_pins emb_fifo_gen_7/din]
  connect_bd_net -net emmetcore_3_rx_gearboxslip [get_bd_pins emmetcore_3/rx_gearboxslip] [get_bd_pins gt_bridge_ip_0/ch3_rxgearboxslip_ext]
  connect_bd_net -net emmetcore_3_rx_polarity [get_bd_pins emmetcore_3/rx_polarity] [get_bd_pins gt_bridge_ip_0/ch3_rxpolarity_ext]
  connect_bd_net -net emmetcore_3_rx_valid [get_bd_pins emmetcore_3/rx_valid] [get_bd_pins emb_fifo_gen_7/wr_en]
  connect_bd_net -net emmetcore_3_tx_header_out [get_bd_pins emmetcore_3/tx_header_out] [get_bd_pins gt_bridge_ip_0/ch3_txheader_ext]
  connect_bd_net -net emmetcore_3_tx_ready [get_bd_pins emmetcore_3/tx_ready] [get_bd_pins emb_fifo_gen_6/rd_en]
  connect_bd_net -net emmetcore_3_tx_sequence_out [get_bd_pins emmetcore_3/tx_sequence_out] [get_bd_pins gt_bridge_ip_0/ch3_txsequence_ext]
  connect_bd_net -net emmetcore_3_tx_userdata_out [get_bd_pins emmetcore_3/tx_userdata_out] [get_bd_pins gt_bridge_ip_0/ch3_txdata_ext]
  connect_bd_net -net gt_bridge_ip_0_ch0_rxdata_ext [get_bd_pins gt_bridge_ip_0/ch0_rxdata_ext] [get_bd_pins emmetcore_0/rx_userdata_in]
  connect_bd_net -net gt_bridge_ip_0_ch0_rxdatavalid_ext [get_bd_pins gt_bridge_ip_0/ch0_rxdatavalid_ext] [get_bd_pins emmetcore_0/rx_datavalid_in]
  connect_bd_net -net gt_bridge_ip_0_ch0_rxheadervalid_ext [get_bd_pins gt_bridge_ip_0/ch0_rxheadervalid_ext] [get_bd_pins emmetcore_0/rx_headervalid_in]
  connect_bd_net -net gt_bridge_ip_0_ch1_rxdata_ext [get_bd_pins gt_bridge_ip_0/ch1_rxdata_ext] [get_bd_pins emmetcore_1/rx_userdata_in]
  connect_bd_net -net gt_bridge_ip_0_ch1_rxdatavalid_ext [get_bd_pins gt_bridge_ip_0/ch1_rxdatavalid_ext] [get_bd_pins emmetcore_1/rx_datavalid_in]
  connect_bd_net -net gt_bridge_ip_0_ch1_rxheader_ext [get_bd_pins gt_bridge_ip_0/ch1_rxheader_ext] [get_bd_pins emmetcore_1/rx_header_in]
  connect_bd_net -net gt_bridge_ip_0_ch1_rxheadervalid_ext [get_bd_pins gt_bridge_ip_0/ch1_rxheadervalid_ext] [get_bd_pins emmetcore_1/rx_headervalid_in]
  connect_bd_net -net gt_bridge_ip_0_ch2_rxdata_ext [get_bd_pins gt_bridge_ip_0/ch2_rxdata_ext] [get_bd_pins emmetcore_2/rx_userdata_in]
  connect_bd_net -net gt_bridge_ip_0_ch2_rxdatavalid_ext [get_bd_pins gt_bridge_ip_0/ch2_rxdatavalid_ext] [get_bd_pins emmetcore_2/rx_datavalid_in]
  connect_bd_net -net gt_bridge_ip_0_ch2_rxheader_ext [get_bd_pins gt_bridge_ip_0/ch2_rxheader_ext] [get_bd_pins emmetcore_2/rx_header_in]
  connect_bd_net -net gt_bridge_ip_0_ch2_rxheadervalid_ext [get_bd_pins gt_bridge_ip_0/ch2_rxheadervalid_ext] [get_bd_pins emmetcore_2/rx_headervalid_in]
  connect_bd_net -net gt_bridge_ip_0_ch3_rxdata_ext [get_bd_pins gt_bridge_ip_0/ch3_rxdata_ext] [get_bd_pins emmetcore_3/rx_userdata_in]
  connect_bd_net -net gt_bridge_ip_0_ch3_rxdatavalid_ext [get_bd_pins gt_bridge_ip_0/ch3_rxdatavalid_ext] [get_bd_pins emmetcore_3/rx_datavalid_in]
  connect_bd_net -net gt_bridge_ip_0_ch3_rxheader_ext [get_bd_pins gt_bridge_ip_0/ch3_rxheader_ext] [get_bd_pins emmetcore_3/rx_header_in]
  connect_bd_net -net gt_bridge_ip_0_ch3_rxheadervalid_ext [get_bd_pins gt_bridge_ip_0/ch3_rxheadervalid_ext] [get_bd_pins emmetcore_3/rx_headervalid_in]
  connect_bd_net -net gt_bridge_ip_0_gt_ilo_reset [get_bd_pins gt_bridge_ip_0/gt_ilo_reset] [get_bd_pins gt_quad_base_0/ch0_iloreset] [get_bd_pins gt_quad_base_0/ch1_iloreset] [get_bd_pins gt_quad_base_0/ch2_iloreset] [get_bd_pins gt_quad_base_0/ch3_iloreset]
  connect_bd_net -net gt_bridge_ip_0_gt_pll_reset [get_bd_pins gt_bridge_ip_0/gt_pll_reset] [get_bd_pins gt_quad_base_0/hsclk0_lcpllreset] [get_bd_pins gt_quad_base_0/hsclk0_rpllreset] [get_bd_pins gt_quad_base_0/hsclk1_lcpllreset] [get_bd_pins gt_quad_base_0/hsclk1_rpllreset]
  connect_bd_net -net gt_bridge_ip_0_pcie_rstb [get_bd_pins gt_bridge_ip_0/pcie_rstb] [get_bd_pins gt_quad_base_0/ch0_pcierstb] [get_bd_pins gt_quad_base_0/ch1_pcierstb] [get_bd_pins gt_quad_base_0/ch2_pcierstb] [get_bd_pins gt_quad_base_0/ch3_pcierstb]
  connect_bd_net -net gt_bridge_ip_0_rx_resetdone_out [get_bd_pins gt_bridge_ip_0/rx_resetdone_out] [get_bd_pins emmetcore_0/rx_resetdone] [get_bd_pins emmetcore_1/rx_resetdone] [get_bd_pins emmetcore_2/rx_resetdone] [get_bd_pins emmetcore_3/rx_resetdone]
  connect_bd_net -net gt_bridge_ip_0_tx_resetdone_out [get_bd_pins gt_bridge_ip_0/tx_resetdone_out] [get_bd_pins emmetcore_0/tx_resetdone] [get_bd_pins emmetcore_1/tx_resetdone] [get_bd_pins emmetcore_2/tx_resetdone] [get_bd_pins emmetcore_3/tx_resetdone]
  connect_bd_net -net gt_quad_base_0_ch0_iloresetdone [get_bd_pins gt_quad_base_0/ch0_iloresetdone] [get_bd_pins xlconcat_2/In0]
  connect_bd_net -net gt_quad_base_0_ch0_phystatus [get_bd_pins gt_quad_base_0/ch0_phystatus] [get_bd_pins xlconcat_3/In0]
  connect_bd_net -net gt_quad_base_0_ch0_rxoutclk [get_bd_pins gt_quad_base_0/ch0_rxoutclk] [get_bd_pins bufg_gt_1/outclk]
  connect_bd_net -net gt_quad_base_0_ch0_txoutclk [get_bd_pins gt_quad_base_0/ch0_txoutclk] [get_bd_pins bufg_gt_0/outclk]
  connect_bd_net -net gt_quad_base_0_ch1_iloresetdone [get_bd_pins gt_quad_base_0/ch1_iloresetdone] [get_bd_pins xlconcat_2/In1]
  connect_bd_net -net gt_quad_base_0_ch1_phystatus [get_bd_pins gt_quad_base_0/ch1_phystatus] [get_bd_pins xlconcat_3/In1]
  connect_bd_net -net gt_quad_base_0_ch2_iloresetdone [get_bd_pins gt_quad_base_0/ch2_iloresetdone] [get_bd_pins xlconcat_2/In2]
  connect_bd_net -net gt_quad_base_0_ch2_phystatus [get_bd_pins gt_quad_base_0/ch2_phystatus] [get_bd_pins xlconcat_3/In2]
  connect_bd_net -net gt_quad_base_0_ch3_iloresetdone [get_bd_pins gt_quad_base_0/ch3_iloresetdone] [get_bd_pins xlconcat_2/In3]
  connect_bd_net -net gt_quad_base_0_ch3_phystatus [get_bd_pins gt_quad_base_0/ch3_phystatus] [get_bd_pins xlconcat_3/In3]
  connect_bd_net -net gt_quad_base_0_gtpowergood [get_bd_pins gt_quad_base_0/gtpowergood] [get_bd_pins emmetcore_0/gt_powergood] [get_bd_pins emmetcore_1/gt_powergood] [get_bd_pins emmetcore_2/gt_powergood] [get_bd_pins emmetcore_3/gt_powergood] [get_bd_pins gt_bridge_ip_0/gtpowergood]
  connect_bd_net -net gt_quad_base_0_hsclk0_lcplllock [get_bd_pins gt_quad_base_0/hsclk0_lcplllock] [get_bd_pins xlconcat_0/In0]
  connect_bd_net -net gt_quad_base_0_hsclk0_rplllock [get_bd_pins gt_quad_base_0/hsclk0_rplllock] [get_bd_pins xlconcat_1/In0]
  connect_bd_net -net gt_quad_base_0_hsclk1_lcplllock [get_bd_pins gt_quad_base_0/hsclk1_lcplllock] [get_bd_pins xlconcat_0/In1]
  connect_bd_net -net gt_quad_base_0_hsclk1_rplllock [get_bd_pins gt_quad_base_0/hsclk1_rplllock] [get_bd_pins xlconcat_1/In1]
  connect_bd_net -net inverter_0_rx_header_out [get_bd_pins gt_bridge_ip_0/ch0_rxheader_ext] [get_bd_pins emmetcore_0/rx_header_in]
  connect_bd_net -net rec_bram_douta [get_bd_pins rec_bram/douta] [get_bd_pins axi_bram_ctrl_rec/bram_rddata_a]
  connect_bd_net -net rec_bram_doutb [get_bd_pins rec_bram/doutb] [get_bd_pins FSM_Rec_from_FPGA/data_bram_i]
  connect_bd_net -net root_hub_core_0_controller_state [get_bd_pins root_hub_core_0/controller_state] [get_bd_pins axis_ila_1/probe16]
  connect_bd_net -net root_hub_core_0_data_from_controller [get_bd_pins root_hub_core_0/data_from_controller] [get_bd_pins axis_ila_1/probe17]
  connect_bd_net -net root_hub_core_0_router_busy [get_bd_pins root_hub_core_0/router_busy] [get_bd_pins axis_ila_1/probe9]
  connect_bd_net -net root_hub_core_0_rx_0_rd_en [get_bd_pins root_hub_core_0/rx_0_rd_en] [get_bd_pins axis_ila_1/probe2] [get_bd_pins CPU_to_FPGA/rd_en]
  connect_bd_net -net root_hub_core_0_rx_1_rd_en [get_bd_pins root_hub_core_0/rx_1_rd_en] [get_bd_pins emb_fifo_gen_1/rd_en] [get_bd_pins axis_ila_1/probe5]
  connect_bd_net -net root_hub_core_0_rx_2_rd_en [get_bd_pins root_hub_core_0/rx_2_rd_en] [get_bd_pins emb_fifo_gen_3/rd_en] [get_bd_pins axis_ila_1/probe8]
  connect_bd_net -net root_hub_core_0_rx_3_rd_en [get_bd_pins root_hub_core_0/rx_3_rd_en] [get_bd_pins emb_fifo_gen_5/rd_en] [get_bd_pins axis_ila_1/probe18]
  connect_bd_net -net root_hub_core_0_rx_4_rd_en [get_bd_pins root_hub_core_0/rx_4_rd_en] [get_bd_pins emb_fifo_gen_7/rd_en] [get_bd_pins axis_ila_1/probe19]
  connect_bd_net -net root_hub_core_0_tx_0_dout [get_bd_pins root_hub_core_0/tx_0_dout] [get_bd_pins axis_ila_1/probe1]
  connect_bd_net -net root_hub_core_0_tx_0_wr_en [get_bd_pins root_hub_core_0/tx_0_wr_en] [get_bd_pins FPGA_to_CPU/wr_en]
  connect_bd_net -net root_hub_core_0_tx_1_dout [get_bd_pins root_hub_core_0/tx_1_dout] [get_bd_pins axis_ila_1/probe3] [get_bd_pins emb_fifo_gen_0/din]
  connect_bd_net -net root_hub_core_0_tx_1_wr_en [get_bd_pins root_hub_core_0/tx_1_wr_en] [get_bd_pins axis_ila_1/probe4] [get_bd_pins emb_fifo_gen_0/wr_en]
  connect_bd_net -net root_hub_core_0_tx_2_dout [get_bd_pins root_hub_core_0/tx_2_dout] [get_bd_pins axis_ila_1/probe6] [get_bd_pins emb_fifo_gen_2/din]
  connect_bd_net -net root_hub_core_0_tx_2_wr_en [get_bd_pins root_hub_core_0/tx_2_wr_en] [get_bd_pins emb_fifo_gen_2/wr_en] [get_bd_pins axis_ila_1/probe7]
  connect_bd_net -net root_hub_core_0_tx_3_dout [get_bd_pins root_hub_core_0/tx_3_dout] [get_bd_pins emb_fifo_gen_4/din]
  connect_bd_net -net root_hub_core_0_tx_3_wr_en [get_bd_pins root_hub_core_0/tx_3_wr_en] [get_bd_pins emb_fifo_gen_4/wr_en] [get_bd_pins axis_ila_1/probe22]
  connect_bd_net -net root_hub_core_0_tx_4_dout [get_bd_pins root_hub_core_0/tx_4_dout] [get_bd_pins emb_fifo_gen_6/din]
  connect_bd_net -net root_hub_core_0_tx_4_wr_en [get_bd_pins root_hub_core_0/tx_4_wr_en] [get_bd_pins emb_fifo_gen_6/wr_en] [get_bd_pins axis_ila_1/probe23]
  connect_bd_net -net root_hub_core_0_valid_from_controller [get_bd_pins root_hub_core_0/valid_from_controller] [get_bd_pins axis_ila_1/probe11]
  connect_bd_net -net rst_versal_cips_0_333M_peripheral_aresetn [get_bd_pins rst_versal_cips_0_333M/peripheral_aresetn] [get_bd_pins axi_bram_ctrl_send/s_axi_aresetn] [get_bd_pins axi_bram_ctrl_status/s_axi_aresetn] [get_bd_pins axi_bram_ctrl_rec/s_axi_aresetn]
  connect_bd_net -net rst_versal_cips_0_333M_peripheral_reset [get_bd_pins rst_versal_cips_0_333M/peripheral_reset] [get_bd_pins CPU_to_FPGA/rst] [get_bd_pins FPGA_to_CPU/rst] [get_bd_pins FSM_Send_to_FPGA/reset] [get_bd_pins FSM_Rec_from_FPGA/reset] [get_bd_pins FSM_Mul_Root_0/reset]
  connect_bd_net -net send_bram_douta [get_bd_pins send_bram/douta] [get_bd_pins axi_bram_ctrl_send/bram_rddata_a]
  connect_bd_net -net send_bram_doutb [get_bd_pins send_bram/doutb] [get_bd_pins FSM_Send_to_FPGA/data_bram_i]
  connect_bd_net -net status_bram_douta [get_bd_pins status_bram/douta] [get_bd_pins axi_bram_ctrl_status/bram_rddata_a]
  connect_bd_net -net status_bram_doutb [get_bd_pins status_bram/doutb] [get_bd_pins FSM_Mul_Root_0/data_bram_i]
  connect_bd_net -net util_ds_buf_0_IBUF_OUT [get_bd_pins util_ds_buf_0/IBUF_OUT] [get_bd_pins gt_quad_base_0/GT_REFCLK0]
  connect_bd_net -net util_reduced_logic_0_Res [get_bd_pins util_reduced_logic_0/Res] [get_bd_pins gt_bridge_ip_0/gt_lcpll_lock]
  connect_bd_net -net util_reduced_logic_1_Res [get_bd_pins util_reduced_logic_1/Res] [get_bd_pins gt_bridge_ip_0/ilo_resetdone]
  connect_bd_net -net util_reduced_logic_2_Res [get_bd_pins util_reduced_logic_2/Res] [get_bd_pins gt_bridge_ip_0/gt_rpll_lock]
  connect_bd_net -net util_vector_logic_0_Res [get_bd_pins util_vector_logic_0/Res] [get_bd_pins root_hub_core_0/reset]
  connect_bd_net -net util_vector_logic_1_Res [get_bd_pins util_vector_logic_1/Res] [get_bd_pins emmetcore_0/tx_valid]
  connect_bd_net -net util_vector_logic_2_Res [get_bd_pins util_vector_logic_2/Res] [get_bd_pins emmetcore_1/tx_valid]
  connect_bd_net -net util_vector_logic_3_Res [get_bd_pins util_vector_logic_3/Res] [get_bd_pins emmetcore_2/tx_valid]
  connect_bd_net -net util_vector_logic_4_Res [get_bd_pins util_vector_logic_4/Res] [get_bd_pins emmetcore_3/tx_valid]
  connect_bd_net -net versal_cips_0_fpd_cci_noc_axi0_clk [get_bd_pins versal_cips_0/fpd_cci_noc_axi0_clk] [get_bd_pins axi_noc_0/aclk0]
  connect_bd_net -net versal_cips_0_fpd_cci_noc_axi1_clk [get_bd_pins versal_cips_0/fpd_cci_noc_axi1_clk] [get_bd_pins axi_noc_0/aclk1]
  connect_bd_net -net versal_cips_0_fpd_cci_noc_axi2_clk [get_bd_pins versal_cips_0/fpd_cci_noc_axi2_clk] [get_bd_pins axi_noc_0/aclk2]
  connect_bd_net -net versal_cips_0_fpd_cci_noc_axi3_clk [get_bd_pins versal_cips_0/fpd_cci_noc_axi3_clk] [get_bd_pins axi_noc_0/aclk3]
  connect_bd_net -net versal_cips_0_hsm1_ref_clk [get_bd_pins versal_cips_0/hsm1_ref_clk] [get_bd_pins axi_noc_0/sys_clk0]
  connect_bd_net -net versal_cips_0_lpd_axi_noc_clk [get_bd_pins versal_cips_0/lpd_axi_noc_clk] [get_bd_pins axi_noc_0/aclk4]
  connect_bd_net -net versal_cips_0_pl0_ref_clk [get_bd_pins versal_cips_0/pl0_ref_clk] [get_bd_pins axi_bram_ctrl_send/s_axi_aclk] [get_bd_pins axi_bram_ctrl_status/s_axi_aclk] [get_bd_pins axi_bram_ctrl_rec/s_axi_aclk] [get_bd_pins CPU_to_FPGA/wr_clk] [get_bd_pins FPGA_to_CPU/wr_clk] [get_bd_pins axi_noc_0/aclk6] [get_bd_pins rst_versal_cips_0_333M/slowest_sync_clk] [get_bd_pins FSM_Send_to_FPGA/clk] [get_bd_pins FSM_Rec_from_FPGA/clk] [get_bd_pins FSM_Mul_Root_0/clk]
  connect_bd_net -net versal_cips_0_pl0_resetn [get_bd_pins versal_cips_0/pl0_resetn] [get_bd_pins rst_versal_cips_0_333M/ext_reset_in]
  connect_bd_net -net versal_cips_0_pmc_axi_noc_axi0_clk [get_bd_pins versal_cips_0/pmc_axi_noc_axi0_clk] [get_bd_pins axi_noc_0/aclk5]
  connect_bd_net -net xlconcat_0_dout [get_bd_pins xlconcat_0/dout] [get_bd_pins util_reduced_logic_0/Op1]
  connect_bd_net -net xlconcat_1_dout [get_bd_pins xlconcat_1/dout] [get_bd_pins util_reduced_logic_2/Op1]
  connect_bd_net -net xlconcat_2_dout [get_bd_pins xlconcat_2/dout] [get_bd_pins util_reduced_logic_1/Op1]
  connect_bd_net -net xlconcat_3_dout [get_bd_pins xlconcat_3/dout] [get_bd_pins gt_bridge_ip_0/ch_phystatus_in]
  connect_bd_net -net xlconstant_0_dout [get_bd_pins xlconstant_0/dout] [get_bd_pins send_bram/enb] [get_bd_pins rec_bram/enb] [get_bd_pins status_bram/enb]

  # Create address segments
  assign_bd_address -offset 0x020200000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_0] [get_bd_addr_segs axi_bram_ctrl_rec/S_AXI/Mem0] -force
  assign_bd_address -offset 0x020100000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_0] [get_bd_addr_segs axi_bram_ctrl_send/S_AXI/Mem0] -force
  assign_bd_address -offset 0x020180000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_0] [get_bd_addr_segs axi_bram_ctrl_status/S_AXI/Mem0] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_0] [get_bd_addr_segs axi_noc_0/S00_AXI/C3_DDR_LOW0] -force
  assign_bd_address -offset 0x000800000000 -range 0x000180000000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_0] [get_bd_addr_segs axi_noc_0/S00_AXI/C3_DDR_LOW1] -force
  assign_bd_address -offset 0x020200000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_1] [get_bd_addr_segs axi_bram_ctrl_rec/S_AXI/Mem0] -force
  assign_bd_address -offset 0x020100000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_1] [get_bd_addr_segs axi_bram_ctrl_send/S_AXI/Mem0] -force
  assign_bd_address -offset 0x020180000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_1] [get_bd_addr_segs axi_bram_ctrl_status/S_AXI/Mem0] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_1] [get_bd_addr_segs axi_noc_0/S01_AXI/C2_DDR_LOW0] -force
  assign_bd_address -offset 0x000800000000 -range 0x000180000000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_1] [get_bd_addr_segs axi_noc_0/S01_AXI/C2_DDR_LOW1] -force
  assign_bd_address -offset 0x020200000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_2] [get_bd_addr_segs axi_bram_ctrl_rec/S_AXI/Mem0] -force
  assign_bd_address -offset 0x020100000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_2] [get_bd_addr_segs axi_bram_ctrl_send/S_AXI/Mem0] -force
  assign_bd_address -offset 0x020180000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_2] [get_bd_addr_segs axi_bram_ctrl_status/S_AXI/Mem0] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_2] [get_bd_addr_segs axi_noc_0/S02_AXI/C0_DDR_LOW0] -force
  assign_bd_address -offset 0x000800000000 -range 0x000180000000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_2] [get_bd_addr_segs axi_noc_0/S02_AXI/C0_DDR_LOW1] -force
  assign_bd_address -offset 0x020200000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_3] [get_bd_addr_segs axi_bram_ctrl_rec/S_AXI/Mem0] -force
  assign_bd_address -offset 0x020100000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_3] [get_bd_addr_segs axi_bram_ctrl_send/S_AXI/Mem0] -force
  assign_bd_address -offset 0x020180000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_3] [get_bd_addr_segs axi_bram_ctrl_status/S_AXI/Mem0] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_3] [get_bd_addr_segs axi_noc_0/S03_AXI/C1_DDR_LOW0] -force
  assign_bd_address -offset 0x000800000000 -range 0x000180000000 -target_address_space [get_bd_addr_spaces versal_cips_0/FPD_CCI_NOC_3] [get_bd_addr_segs axi_noc_0/S03_AXI/C1_DDR_LOW1] -force
  assign_bd_address -offset 0x020200000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces versal_cips_0/LPD_AXI_NOC_0] [get_bd_addr_segs axi_bram_ctrl_rec/S_AXI/Mem0] -force
  assign_bd_address -offset 0x020100000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces versal_cips_0/LPD_AXI_NOC_0] [get_bd_addr_segs axi_bram_ctrl_send/S_AXI/Mem0] -force
  assign_bd_address -offset 0x020180000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces versal_cips_0/LPD_AXI_NOC_0] [get_bd_addr_segs axi_bram_ctrl_status/S_AXI/Mem0] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces versal_cips_0/LPD_AXI_NOC_0] [get_bd_addr_segs axi_noc_0/S04_AXI/C3_DDR_LOW0] -force
  assign_bd_address -offset 0x000800000000 -range 0x000180000000 -target_address_space [get_bd_addr_spaces versal_cips_0/LPD_AXI_NOC_0] [get_bd_addr_segs axi_noc_0/S04_AXI/C3_DDR_LOW1] -force
  assign_bd_address -offset 0x020200000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces versal_cips_0/PMC_NOC_AXI_0] [get_bd_addr_segs axi_bram_ctrl_rec/S_AXI/Mem0] -force
  assign_bd_address -offset 0x020100000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces versal_cips_0/PMC_NOC_AXI_0] [get_bd_addr_segs axi_bram_ctrl_send/S_AXI/Mem0] -force
  assign_bd_address -offset 0x020180000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces versal_cips_0/PMC_NOC_AXI_0] [get_bd_addr_segs axi_bram_ctrl_status/S_AXI/Mem0] -force
  assign_bd_address -offset 0x00000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces versal_cips_0/PMC_NOC_AXI_0] [get_bd_addr_segs axi_noc_0/S05_AXI/C2_DDR_LOW0] -force
  assign_bd_address -offset 0x000800000000 -range 0x000180000000 -target_address_space [get_bd_addr_spaces versal_cips_0/PMC_NOC_AXI_0] [get_bd_addr_segs axi_noc_0/S05_AXI/C2_DDR_LOW1] -force


  # Restore current instance
  current_bd_instance $oldCurInst

  validate_bd_design
  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""


