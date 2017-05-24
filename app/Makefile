CPP 		= g++
CXXFLAGS	= -g -O3 -Wall -fPIC -D_REENTRANT -Wno-deprecated

ROOTCFLAGS	:= $(shell root-config --cflags)
ROOTLIBS     	:= $(shell root-config --libs)
ROOTGLIBS    	:= $(shell root-config --glibs)
CXXFLAGS	+= $(ROOTCFLAGS)

#LIB=/home/kikawa/PM_Ana/lib
LIB=/home/cvson/cc1picoh/frkikawa/PM_Ana/lib
#LIB=/home/ingrid/ingrid_format
LIBS 		= $(ROOTLIBS) -lm $(LIB)/INGRIDEVENTSUMMARY.so $(LIB)/IngridHitSummary.so $(LIB)/IngridSimHitSummary.so $(LIB)/IngridSimVertexSummary.so $(LIB)/IngridSimParticleSummary.so $(LIB)/BeamInfoSummary.so $(LIB)/IngridBasicReconSummary.so $(LIB)/PMReconSummary.so $(LIB)/PMAnaSummary.so $(LIB)/Ingrid1stReducSummary.so $(LIB)/IngridTrackSummary.so $(LIB)/NeutInfoSummary.so
#LIBS 		= $(ROOTLIBS) -lm $(LIB)/INGRIDEVENTSUMMARY.so $(LIB)/IngridHitSummary.so $(LIB)/IngridSimHitSummary.so $(LIB)/IngridSimVertexSummary.so $(LIB)/IngridSimParticleSummary.so $(LIB)/BeamInfoSummary.so $(LIB)/IngridBasicReconSummary.so $(LIB)/Ingrid1stReducSummary.so $(LIB)/IngridTrackSummary.so

GLIBS		= $(ROOTGLIBS)

#SRC=/home/kikawa/PM_Ana/src
#INC=/home/kikawa/PM_Ana/inc

SRC=/home/cvson/cc1picoh/frkikawa/PM_Ana/src
INC=/home/cvson/cc1picoh/frkikawa/PM_Ana/inc

#TARGET=IngReTrackTest
#OBJS=IngReTrackTest.o

#TARGET=IngAddWeight
#OBJS=IngAddWeight.o

#TARGET=IngSysSeveralFid
#OBJS=IngSysSeveralFid.o

#TARGET=pro_several_fid
#OBJS=pro_several_fid.o

#TARGET=get_profile_2
#OBJS=get_profile_2.o

#TARGET=Get_TrkEff4
#OBJS=Get_TrkEff4.o

#TARGET=IngCheckXsec
#OBJS=IngCheckXsec.o

#TARGET=IngAnaCosmicTrackingResult
#OBJS=IngAnaCosmicTrackingResult.o

#TARGET=IngAnaRmuTracking
#OBJS=IngAnaRmuTracking.o

#TARGET=check_occupancy
#OBJS=check_occupancy.o

#TARGET=Check110117
#OBJS=Check110117.o

#TARGET=plot_sim_angle
#OBJS=plot_sim_angle.o

#TARGET=MakeDQ_t
#OBJS=MakeDQ_t.o

#TARGET=IngReconMuBasic_rev
#OBJS=IngReconMuBasic_rev.o
TARGET=IngGetHit
OBJS=IngGetHit.o
TARGET=Eff
OBJS=Eff.o
TARGET=IngRmTrackMC
OBJS=IngRmTrackMC.o
#TARGET=IngSumHit
#OBJS=IngSumHit.o
#TARGET=IngSumHit3
#OBJS=IngSumHit3.o
TARGET=IngAddNoiseMCtmp
OBJS=IngAddNoiseMCtmp.o
TARGET=Effwoneut
OBJS=Effwoneut.o
TARGET=PMrecontmp
OBJS=PMrecontmp.o
TARGET=PMrecon_simple
OBJS=PMrecon_simple.o
TARGET=PMrecon_new
OBJS=PMrecon_new.o
#TARGET=PMrecon_old
#OBJS=PMrecon_old.o
#TARGET=IngShowVertex
#OBJS=IngShowVertex.o
TARGET=Event_selection_mod
OBJS=Event_selection_mod.o
TARGET=IngPile_new_tmp
OBJS=IngPile_new_tmp.o
TARGET=IngPile_new_tmp2
OBJS=IngPile_new_tmp2.o
TARGET=IngPile
OBJS=IngPile.o
#TARGET=IngPile_new
#OBJS=IngPile_new.o
TARGET=IngPiletmp_new
OBJS=IngPiletmp_new.o
TARGET=IngCutEmpPM
OBJS=IngCutEmpPM.o
TARGET=IngSumHit2
OBJS=IngSumHit2.o
TARGET=IngCompare
OBJS=IngCompare.o
TARGET=IngEvtDisp
OBJS=IngEvtDisp.o
TARGET=IngGetOnlyBasic2
OBJS=IngGetOnlyBasic2.o
TARGET=IngDQ
OBJS=IngDQ.o
TARGET=IngSumHitsimple
OBJS=IngSumHitsimple.o
TARGET=IngGetOnlyBasicMC_neut
OBJS=IngGetOnlyBasicMC_neut.o
TARGET=Event_selection
OBJS=Event_selection.o
TARGET=potmurate_spill
OBJS=potmurate_spill.o
TARGET=IngPiletmp
OBJS=IngPiletmp.o
TARGET=IngRmTrack_not
OBJS=IngRmTrack_not.o
TARGET=IngAnaCosmicTracking_div
OBJS=IngAnaCosmicTracking_div.o
TARGET=IngAnaCosmicTrackingGetOnlyBasic_sel
OBJS=IngAnaCosmicTrackingGetOnlyBasic_sel.o
TARGET=get_daily_evt_limit_ppb
OBJS=get_daily_evt_limit_ppb.o
TARGET=get_daily_evt_corr_limit_ppb
OBJS=get_daily_evt_corr_limit_ppb.o
TARGET=EvtDisp_dummy
OBJS=EvtDisp_dummy.o
TARGET=IngGetOnlyBasic_div
OBJS=IngGetOnlyBasic_div.o
TARGET=IngGetOnlyPMRecon
OBJS=IngGetOnlyPMRecon.o
TARGET=IngGetOnlyPMReconMC
OBJS=IngGetOnlyPMReconMC.o
TARGET=IngAddNoiseMChist
OBJS=IngAddNoiseMChist.o
TARGET=IngLimitPPB
OBJS=IngLimitPPB.o
TARGET=potevtrate
OBJS=potevtrate.o
TARGET=IngNpile
OBJS=IngNpile.o
TARGET=get_daily_evt
OBJS=get_daily_evt.o
TARGET=potevtrate_corr
OBJS=potevtrate_corr.o
TARGET=IngNoiseRate
OBJS=IngNoiseRate.o
TARGET=IngNoiseRatePPB
OBJS=IngNoiseRatePPB.o
TARGET=get_daily_evt_corr2
OBJS=get_daily_evt_corr2.o
TARGET=IngAnaCosmicTracking
OBJS=IngAnaCosmicTracking.o
TARGET=IngAnaCosmicTracking_tmp
OBJS=IngAnaCosmicTracking_tmp.o
TARGET=IngAnaCosmicTracking_rev
OBJS=IngAnaCosmicTracking_rev.o
TARGET=IngCutNoise
OBJS=IngCutNoise.o
TARGET=IngAnaCosmicTrackingGetOnlyBasic
OBJS=IngAnaCosmicTrackingGetOnlyBasic.o
TARGET=IngAnaCosmicTrackingGetOnlyBasicMC
OBJS=IngAnaCosmicTrackingGetOnlyBasicMC.o
TARGET=IngRmTrackMCwoneut
OBJS=IngRmTrackMCwoneut.o
TARGET=IngKillDead
OBJS=IngKillDead.o
TARGET=get_daily_evt_rev
OBJS=get_daily_evt_rev.o
TARGET=IngSumHitData
OBJS=IngSumHitData.o
TARGET=IngSumHitDataPPB
OBJS=IngSumHitDataPPB.o
TARGET=PMrecon_tmp
OBJS=PMrecon_tmp.o
TARGET=IngAddTrueAng
OBJS=IngAddTrueAng.o
TARGET=IngAddTrueAng_anti
OBJS=IngAddTrueAng_anti.o
TARGET=IngAnaVertex_onetrack
OBJS=IngAnaVertex_onetrack.o
TARGET=Disp
OBJS=Disp.o
TARGET=PMrecon_wodead
OBJS=PMrecon_wodead.o
TARGET=PMrecon_wdead
OBJS=PMrecon_wdead.o
TARGET=get_daily_evt_corr_rev2
OBJS=get_daily_evt_corr_rev2.o
TARGET=IngGetOnlyBasic
OBJS=IngGetOnlyBasic.o
TARGET=IngAna
OBJS=IngAna.o
TARGET=IngAddNoiseMC_new
OBJS=IngAddNoiseMC_new.o
TARGET=IngRmTrack2
OBJS=IngRmTrack2.o
TARGET=HitEff
OBJS=HitEff.o
TARGET=IngClassMC
OBJS=IngClassMC.o
TARGET=POT_history_BSD
OBJS=POT_history_BSD.o
TARGET=POT_history_ING
OBJS=POT_history_ING.o
TARGET=IngClass
OBJS=IngClass.o
TARGET=IngCutEmp
OBJS=IngCutEmp.o
TARGET=PMGetOnlyBasicMC
OBJS=PMGetOnlyBasicMC.o
TARGET=HitEff
OBJS=HitEff.o
TARGET=HitEffMC
OBJS=HitEffMC.o
TARGET=HitEffPM
OBJS=HitEffPM.o
TARGET=PEdistPMMC
OBJS=PEdistPMMC.o
TARGET=PEdistPM
OBJS=PEdistPM.o
TARGET=PEdistMC
OBJS=PEdistMC.o
TARGET=PEdist
OBJS=PEdist.o
TARGET=PMAna_tmp
OBJS=PMAna_tmp.o
TARGET=PMAna_tmp
OBJS=PMAna_tmp.o
TARGET=IngNVertexMC
OBJS=IngNVertexMC.o
TARGET=PMAnaVertex
OBJS=PMAnaVertex.o
TARGET=IngGetOnlyBasicMC_pm
OBJS=IngGetOnlyBasicMC_pm.o
TARGET=IngNver_pm
OBJS=IngNver_pm.o
TARGET=PMNver
OBJS=PMNver.o
TARGET=IngGetOnlyBasicMC
OBJS=IngGetOnlyBasicMC.o

TARGET=IngTClster
OBJS=IngTClster.o
TARGET=IngBasicRecon
OBJS=IngBasicRecon.o
TARGET=IngReconMuBasic
OBJS=IngReconMuBasic.o
TARGET=PMrecon_disptmp
OBJS=PMrecon_disptmp.o

TARGET=IngGetWeight
OBJS=IngGetWeight.o

TARGET=IngAddCut
OBJS=IngAddCut.o

TARGET=IngAddCutMod3
OBJS=IngAddCutMod3.o

TARGET=IngGetWeightPar
OBJS=IngGetWeightPar.o

TARGET=IngCalcQsquare
OBJS=IngCalcQsquare.o

TARGET=PMrecon_wodead
OBJS=PMrecon_wodead.o

TARGET=PMrecon_offcyc
OBJS=PMrecon_offcyc.o

TARGET=IngAnaVertex_offcyc
OBJS=IngAnaVertex_offcyc.o


TARGET=PMAna_sys
OBJS=PMAna_sys.o

TARGET=PMGetOnlyBasic
OBJS=PMGetOnlyBasic.o

TARGET=get_daily_evt_corr_rev_ic
OBJS=get_daily_evt_corr_rev_ic.o

TARGET=PMAna
OBJS=PMAna.o

TARGET=PMrecon
OBJS=PMrecon.o

TARGET=PEdistPMRev
OBJS=PEdistPMRev.o

TARGET=PEdistCls
OBJS=PEdistCls.o

TARGET=PEdistClsRev
OBJS=PEdistClsRev.o



TARGET=PMAnaRev_sys
OBJS=PMAnaRev_sys.o


TARGET=IngAddNoisePMMC
OBJS=IngAddNoisePMMC.o

TARGET=PMAnaRev
OBJS=PMAnaRev.o

TARGET=IngCutNoise_rev
OBJS=IngCutNoise_rev.o

TARGET=PMAnaRev_hiteff
OBJS=PMAnaRev_hiteff.o

TARGET=HitEffPMMC
OBJS=HitEffPMMC.o


TARGET=IngAnaVertex_sys
OBJS=IngAnaVertex_sys.o

TARGET=get_daily_evt_corr_rev
OBJS=get_daily_evt_corr_rev.o

TARGET=get_daily_evt_pm
OBJS=get_daily_evt_pm.o

TARGET=get_daily_evt_pm_wocorr
OBJS=get_daily_evt_pm_wocorr.o

TARGET=PMreconRevtmp
OBJS=PMreconRevtmp.o

TARGET=PMAnaRevNew
OBJS=PMAnaRevNew.o

TARGET=PMnactpln
OBJS=PMnactpln.o

TARGET=PMCCQEAna
OBJS=PMCCQEAna.o

TARGET=IngSumHitDataPPBPM
OBJS=IngSumHitDataPPBPM.o

TARGET=TrueMuAng
OBJS=TrueMuAng.o

TARGET=PMAnaRev_perev
OBJS=PMAnaRev_perev.o

TARGET=IngAddNoisePMMCRev
OBJS=IngAddNoisePMMCRev.o


TARGET=IngAnaVertex
OBJS=IngAnaVertex.o

TARGET=PMreconRev_wodead
OBJS=PMreconRev_wodead.o

TARGET=IngAddNoiseMC_wodead
OBJS=IngAddNoiseMC_wodead.o

TARGET=PMCCQEAnaRev
OBJS=PMCCQEAnaRev.o

TARGET=IngReduce
OBJS=IngReduce.o


TARGET=PMAnaRevRev_pe
OBJS=PMAnaRevRev_pe.o

TARGET=PMAnaRevRev_hiteff
OBJS=PMAnaRevRev_hiteff.o

TARGET=PMAnaRevRev_sys
OBJS=PMAnaRevRev_sys.o

TARGET=PMAnaRevRev_veract
OBJS=PMAnaRevRev_veract.o

TARGET=get_daily_evt_corr
OBJS=get_daily_evt_corr.o


TARGET=TrkEffSys
OBJS=TrkEffSys.o

TARGET=HitEffMC_topology
OBJS=HitEffMC_topology.o

TARGET=IngMaskHit
OBJS=IngMaskHit.o

TARGET=GetTrueMuAng
OBJS=GetTrueMuAng.o
TARGET=IngShowVertexData
OBJS=IngShowVertexData.o

TARGET=PMreconRev_offtime
OBJS=PMreconRev_offtime.o

TARGET=PMAnaRevRev
OBJS=PMAnaRevRev.o
TARGET=PMTrueAng
OBJS=PMTrueAng.o


TARGET=PMPEinPM
OBJS=PMPEinPM.o

TARGET=PEdistBen
OBJS=PEdistBen.o

TARGET=PMPEinPMsci
OBJS=PMPEinPMsci.o

TARGET=NeutGetInfo_rev
OBJS=NeutGetInfo_rev.o

TARGET=PMAnaRevRev_veractlast
OBJS=PMAnaRevRev_veractlast.o

TARGET=PMreconRev
OBJS=PMreconRev.o

TARGET=PMCCQEAnaRevKinematicDefence
OBJS=PMCCQEAnaRevKinematicDefence.o

TARGET=PMCCQEAnaRevMuang
OBJS=PMCCQEAnaRevMuang.o

TARGET=PMAnaRevRev_veractlast2
OBJS=PMAnaRevRev_veractlast2.o

TARGET=PMCCQEAnaRevMuPang
OBJS=PMCCQEAnaRevMuPang.o

TARGET=PMCCQEAnaRevMuPangRev
OBJS=PMCCQEAnaRevMuPangRev.o

TARGET=GetBjorkenX
OBJS=GetBjorkenX.o

TARGET=GetNoiseRate
OBJS=GetNoiseRate.o

TARGET=IngAddNoisePMMCRevCoh
OBJS=IngAddNoisePMMCRevCoh.o

TARGET=PMreconRev
OBJS=PMreconRev.o

TARGET=Diffractive
OBJS=Diffractive.o
############CVSON 
TARGET=IngMergePM
OBJS=IngMergePM.o

TARGET=IngAddNoiseMC
OBJS=IngAddNoiseMC.o

TARGET=PMreconRev
OBJS=PMreconRev.o

TARGET=PMAnaRevRev
OBJS=PMAnaRevRev.o

TARGET=PMCCQEAnaRev
OBJS=PMCCQEAnaRev.o

###For systematics
TARGET=PMAnaRevRev_sys
OBJS=PMAnaRevRev_sys.o

TARGET=PMAnaRevRev_pe
OBJS=PMAnaRevRev_pe.o

TARGET=PMAnaRevRev_hiteff
OBJS=PMAnaRevRev_hiteff.o

TARGET=PMAnaRevRev_veract
OBJS=PMAnaRevRev_veract.o

TARGET=MakeClassTest
OBJS=MakeClassTest.o

TARGET=NeutGetInfo
OBJS=NeutGetInfo.o

###For reprocess data
TARGET=PMCCQEAnaRev
OBJS=PMCCQEAnaRev.o

###For add more branch data
#TARGET=PMCCQEAnaRev_cvson
#OBJS=PMCCQEAnaRev_cvson.o

TARGET=GetTrueMuPiAng
OBJS=GetTrueMuPiAng.o

#TARGET=PMAnaRevRev
#OBJS=PMAnaRevRev.o

#TARGET=PMGetOnlyBasic
#OBJS=PMGetOnlyBasic.o

#TARGET=PMCCQEAnaRev
#OBJS=PMCCQEAnaRev.o

#TARGET=PMGetOnlyBasicMC
#OBJS=PMGetOnlyBasicMC.o

TARGET=IngAddNoisePMMCRev
OBJS=IngAddNoisePMMCRev.o

TARGET=PMreconRev
OBJS=PMreconRev.o

#TARGET=IngAnaVertex
#OBJS=IngAnaVertex.o

#TARGET=IngGetOnlyBasicMC
#OBJS=IngGetOnlyBasicMC.o

#TARGET=PMCCQEAnaRev_addTrue
#OBJS=PMCCQEAnaRev_addTrue.o

#TARGET=IngAddNoisePMMCRevDiv
#OBJS=IngAddNoisePMMCRevDiv.o

#TARGET=PMCCQEAnaRev_addTrue
#OBJS=PMCCQEAnaRev_addTrue.o

#TARGET=IngCutEmp
#OBJS=IngCutEmp.o

#TARGET=PMCCQEAnaRev
#OBJS=PMCCQEAnaRev.o

TARGET=PMCCQEAnaRev_addpionFS
OBJS=PMCCQEAnaRev_addpionFS.o

TARGET=PMAnaRevRev_pevar
OBJS=PMAnaRevRev_pevar.o

TARGET=PMCCQEAnaRev_addpionFS
OBJS=PMCCQEAnaRev_addpionFS.o

TARGET=PMCCQEAnaRev_addpionFS_GENIE
OBJS=PMCCQEAnaRev_addpionFS_GENIE.o

TARGET=PMCCQEAnaRev_addpionFS_GENIE_noveto
OBJS=PMCCQEAnaRev_addpionFS_GENIE_noveto.o

#TARGET=PMreconRev_dispMEC
#OBJS=PMreconRev_dispMEC.o

#PMrecon_disptmp.cc
#TARGET=PMrecon_disptmp
#OBJS=PMrecon_disptmp.o

#TARGET=IngAddNoisePMMCRevDiv
#OBJS=IngAddNoisePMMCRevDiv.o


$(TARGET): $(OBJS)
	@echo "Now make $@"
	@$(CPP) -o $@ $(OBJS) $(CXXFLAGS) $(LIBS) -I $(LIB) -I $(SRC) -I $(INC)
	@echo "..Compile done  \(^o^)/"

.cc.o:
	@echo "Start Compiling $<"
	@$(CPP) -c $(CXXFLAGS) $(GDBFLAGS) $< -I $(LIB) -I $(SRC) -I $(INC)
	@echo ".. Compiling Object Files $< -> done."
	@echo ""

clean: 
	@echo "Now Clean Up"
	rm -f $(TARGET) *~ *.o *.o~ core



