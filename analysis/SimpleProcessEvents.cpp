#include <iostream>
#include <math.h>

#include "TFile.h"
#include "TTree.h"

bool is_fully_contained(double x, double y, double z) {

  //punti x y e z 
  double_t xCenter = 0;
  double_t yCenter = 5.0;
  double_t zCenter = 51.4;
  
  //everything in mm
  double GasRadius = 36.9;
  double GasThickness = 50;
  //xz sono sulla circonferenza y è la drift direction

  double_t vertical_distance = abs(y-yCenter);
  double_t distance_from_center = sqrt( pow(xCenter-x,2)+pow(zCenter-z,2)  );
  
  if(vertical_distance<(GasThickness/2-1) && distance_from_center<(GasRadius-1)){
    return true;
  } else {
    return false;
  }

}


int main(int argc, char** argv){

  std::string filename = argv[1];
  
  TFile* f = TFile::Open(filename.c_str());
  
  TTree* tree = (TTree*)f->Get("Hits");

  Int_t Evn,ParticleID,ParticleTag,ParentID,VolumeNumber;
  Double_t x_hits,y_hits,z_hits,VolumeTraslX,VolumeTraslY,VolumeTraslZ,EnergyDeposit;

  Char_t Nucleus[20];

  Char_t ParticleName[20];

  Char_t CreationProcess[20];
  
  tree->SetBranchAddress("EventNumber",&Evn);
  tree->SetBranchAddress("ParticleID",&ParticleID);
  tree->SetBranchAddress("ParticleTag",&ParticleTag);
  tree->SetBranchAddress("ParticleName",&ParticleName);
  tree->SetBranchAddress("ParentID",&ParentID);
  tree->SetBranchAddress("VolumeNumber",&VolumeNumber);
  tree->SetBranchAddress("x_hits",&x_hits);
  tree->SetBranchAddress("y_hits",&y_hits);
  tree->SetBranchAddress("z_hits",&z_hits);
  tree->SetBranchAddress("EnergyDeposit",&EnergyDeposit);
  tree->SetBranchAddress("VolumeNumber",&VolumeNumber);
  tree->SetBranchAddress("VolumeTraslX",&VolumeTraslX);
  tree->SetBranchAddress("VolumeTraslY",&VolumeTraslY);
  tree->SetBranchAddress("VolumeTraslZ",&VolumeTraslZ);
  tree->SetBranchAddress("Nucleus",&Nucleus);
  tree->SetBranchAddress("ProcessType",&CreationProcess);
  
  TFile* fout= new TFile(Form("elab_%s",filename.c_str()),"recreate");
  fout->cd();
   
  Int_t Out_evNumber;
  std::string Out_PartName;
  std::string Out_Nucl;
  std::vector<Double_t> x_hits_out, y_hits_out, z_hits_out, EDepHits_out;
  bool fullyCont;
  Int_t nhits_out;
  Double_t EDep_Out;
  
  std::string Out_Process;
  
  TTree* outTree = new TTree("elabHits","elabHits");
  outTree->Branch("evNumber",&Out_evNumber);
  outTree->Branch("PartName",&Out_PartName);
  outTree->Branch("EDep_Out",&EDep_Out);
  outTree->Branch("Nucleus",&Out_Nucl);
  outTree->Branch("nhits_out",&nhits_out);
  outTree->Branch("x_hits_out",&x_hits_out);
  outTree->Branch("y_hits_out",&y_hits_out);
  outTree->Branch("z_hits_out",&z_hits_out);
  outTree->Branch("Edep_hits",&EDepHits_out);
  outTree->Branch("FullyContained",&fullyCont);
      
  
  Int_t flag=0;
  Int_t Nentries = tree->GetEntries();
    
  tree->GetEntry(0);
  Out_Nucl=Nucleus;
  Out_Process = CreationProcess;
  Out_evNumber = Evn;
  nhits_out=0;
  EDep_Out=0;
  fullyCont=true;

  for(int i=0;i<Nentries;i++){
    tree->GetEntry(i);
    
    if(i%10000==0) std::cout << i <<"/" << Nentries << "\n";
    
    if(strcmp(ParticleName,"e-")!=0 && strcmp(ParticleName,"e+")!=0 && strcmp(ParticleName,"alpha")!=0){
      
      continue;
      
      //check the phot creation process
    }else if( strcmp(CreationProcess,Out_Process.c_str())==0  &&  strcmp(Nucleus,Out_Nucl.c_str())==0  &&  flag == 0 && Out_evNumber == Evn){
      
      Out_evNumber = Evn;
      Out_PartName = ParticleName;
      Out_Nucl=Nucleus;
      
      x_hits_out.push_back(x_hits);
      y_hits_out.push_back(y_hits);
      z_hits_out.push_back(z_hits);	
      EDepHits_out.push_back(EnergyDeposit);
      nhits_out++;
      EDep_Out+=EnergyDeposit;
      
      if(nhits_out>10){
	if(fullyCont) fullyCont = is_fully_contained(x_hits,y_hits,z_hits);
      }
      
    }else if( ((strcmp(CreationProcess,"ionIoni")==0 || strcmp(CreationProcess,"eIoni")==0)) ){
      
      flag=1;
      
      x_hits_out.push_back(x_hits);
      y_hits_out.push_back(y_hits);
      z_hits_out.push_back(z_hits);	
      EDepHits_out.push_back(EnergyDeposit);
      nhits_out++;   
      EDep_Out+=EnergyDeposit;
      
    }else{
      
      outTree->Fill();
      
      flag=0;
      fullyCont=true;
      EDep_Out=0;
      EDepHits_out.clear();  x_hits_out.clear(); y_hits_out.clear(); z_hits_out.clear();
      
      Out_evNumber = Evn;
      Out_PartName = ParticleName;
      Out_Nucl=Nucleus;
      Out_Process=CreationProcess;
      
      x_hits_out.push_back(x_hits);
      y_hits_out.push_back(y_hits);
      z_hits_out.push_back(z_hits);	
      EDepHits_out.push_back(EnergyDeposit);
      nhits_out=1;  
      EDep_Out+=EnergyDeposit;
      
    }
    
  }//chiudo for
  
  //outTree->Write();
  fout->Save();
  fout->Close();

}
