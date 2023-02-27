bool is_fully_contained(double x, double y, double z) {

  double InnerSourceContThick = 8;
  double GasRadius = 40;
  double GasDistanceFromCollim = 0;
  double CollimatorDepth = 0.6;
  double CollimatorDistance = 0;
  double GasThickness = 50;
  
  double cyl_center_z = InnerSourceContThick/2+CollimatorDepth/2+CollimatorDistance+GasRadius+GasDistanceFromCollim; 

  double distance_from_center = std::sqrt(std::pow(x, 2) + std::pow(cyl_center_z-z, 2));
  double vertical_distance = abs(y);

  if(vertical_distance<(GasThickness/2-1) && distance_from_center<(GasRadius-1)){
    return true;
  } else {
    return false;
  }

}



void RecoHits(std::string filename){
  
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

  Int_t Out_event;
  Double_t ETotal;
  Int_t nhits_out;
  std::vector<Double_t> x_hits_out, y_hits_out, z_hits_out, EdepHits_out;
  std::string nucl;
  bool fullyCont;
  
  TFile* f_out = new TFile(Form("elab_%s",filename.c_str()),"recreate");
  TTree* outTree = new TTree("elabHits","elabHits");

  outTree->Branch("EventNumber",&Out_event);
  outTree->Branch("TotalEDep",&ETotal);
  outTree->Branch("nhits",&nhits_out);
  outTree->Branch("x_hits",&x_hits_out);
  outTree->Branch("y_hits",&y_hits_out);
  outTree->Branch("z_hits",&z_hits_out);
  outTree->Branch("Edep_hits",&EdepHits_out);
  outTree->Branch("FullyContained",&fullyCont);

  tree->GetEntry(0);
  Out_event=Evn;
  nucl=Nucleus;
  ETotal=0;
  nhits_out=0;
  fullyCont=true;
  
  for(int i=0; i<tree->GetEntries(); i++){

    if(i%10000==0)std::cout << i << "/" << tree->GetEntries() << std::endl;

    tree->GetEntry(i);

    if(Out_event == Evn && nucl == Nucleus){
      ETotal += EnergyDeposit;
      x_hits_out.push_back(x_hits);
      y_hits_out.push_back(y_hits);
      z_hits_out.push_back(z_hits);
      EdepHits_out.push_back(EnergyDeposit);
      nhits_out++;
      if(nhits_out>50){
	if(fullyCont) fullyCont = is_fully_contained(x_hits,y_hits,z_hits);
      }
    }else{
      outTree->Fill();
      
      Out_event=Evn;
      nucl=Nucleus;
      ETotal=0;
      nhits_out=1;
      x_hits_out.clear(); y_hits_out.clear(); z_hits_out.clear(); EdepHits_out.clear();
      
      ETotal+=EnergyDeposit;
      x_hits_out.push_back(x_hits);
      y_hits_out.push_back(y_hits);
      z_hits_out.push_back(z_hits);
      EdepHits_out.push_back(EnergyDeposit);
      fullyCont=true;
    }//chioudo if
    
  }//chiudo for entrate

  f_out->cd();
  outTree->Write();
  f_out->Save();
  f_out->Close();
  
}
