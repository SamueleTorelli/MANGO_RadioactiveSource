#include <TTree.h>
#include <TFile.h>
#include <TFile.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TGraph.h>
#include <TF1.h>
#include <TMath.h>
#include <TH1D.h>
#include <memory>
#include <algorithm>
#include <iostream>
#include <vector>
#include <iterator>
#include <cmath>

int main(int argc, char** argv){

  auto f = std::unique_ptr<TFile>( TFile::Open(Form("%s",argv[1]),"r") );
  
  auto mytree = std::unique_ptr<TTree>( (TTree*)f.get()->Get("elabHits") );
    
  TTreeReader reader(mytree.get());
  TTreeReaderValue<Int_t> nhits(reader, "nhits_out");
  TTreeReaderValue<Double_t> EDepTot(reader, "EDep_Out");
  TTreeReaderValue<std::vector<double>> x_hits(reader, "x_hits_out");
  TTreeReaderValue<std::vector<double>> y_hits(reader, "y_hits_out");
  TTreeReaderValue<std::vector<double>> z_hits(reader, "z_hits_out");
  TTreeReaderValue<std::vector<double>> edep_hits(reader, "Edep_hits");

  std::unique_ptr<TGraph> graph(new TGraph());
  std::unique_ptr<TF1> fit( new TF1("fit","pol1") );

  std::vector<Double_t> X,Y;

  //1.2cm is the fitted distance 

  std::unique_ptr<TH1D> histoTh( new TH1D("histoIntrRes","histoIntrRes",150,-TMath::Pi(),TMath::Pi()) );
  
  std::vector<TGraph*> TGvec;
  
  double dist=0;
  double dir;

  auto fout = std::unique_ptr<TFile>( new TFile("outHisto.root","recreate") );  
  fout.get()->cd();

  std::unique_ptr<TTree> myTree( new TTree("myTree", "My Tree") );

  Double_t EDepOut;
  
  myTree->Branch("EDepOut", &EDepOut, "EDepOut/D");
  myTree->Branch("Dir", &dir, "Dir/D");
    

  for (auto entry : reader){  
    
    
    for(auto x = (*x_hits).begin(), y = (*z_hits).begin() ; x!= (*x_hits).end()-1 || y!=(*z_hits).end()-1; ++x, ++y   ){
      
      if(dist<20){
	X.push_back((*y));
	Y.push_back((*x));
	//std::cout << (*x) << " "<< (*y) << std::endl;
      } else {
	break;
      }//chiudo else

      dist+=sqrt( ((*x)-(*x+1)) * ((*x)-(*x+1)) + ((*y)-(*y+1)) * ((*y)-(*y+1)) );
      //std::cout << dist << std::endl;
      
    }//chiudo for on coordinates

    dist=0;  

    for(int i=0;i<graph->GetN();i++){
      graph.get()->RemovePoint(i);
    }
    
    for(int i=0;i<X.size();i++){
      graph.get()->SetPoint(i,X[i],Y[i]);
    }
    
    graph.get()->Fit(fit.get(),"Q");

    if(entry%300 == 0) {
      graph.get()->SetName(Form("Plot%lli",entry));
      graph.get()->SetMarkerStyle(8);
      graph.get()->Write();
    }

    dir=atan(fit.get()->GetParameter(1));
    EDepOut=*EDepTot;
    
    histoTh->Fill(atan(fit.get()->GetParameter(1)));
    std:: cout <<  *EDepTot  <<" " <<fit.get()->GetParameter(1) << "  " << atan(fit.get()->GetParameter(1)) << "\n";
    X.clear();
    Y.clear();

    myTree->Fill();
    
  }//chiudo for entries


  histoTh.get()->Write();
  myTree.get()->Write();
  
  fout.get()->Save();
  fout.get()->Close();

  std::cout << "end" << std::endl;
  
  return EXIT_SUCCESS;
}
