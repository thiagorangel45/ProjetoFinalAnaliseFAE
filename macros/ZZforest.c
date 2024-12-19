#include <TChain.h>
#include <TTreeReader.h>
#include <TTreeReaderArray.h>
#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>
#include <iostream>

void ZZforest() {

    std::vector<std::string> diretorios = {
        "/opendata/eos/opendata/cms/Run2016G/DoubleEG/NANOAOD/UL2016_MiniAODv2_NanoAODv9-v1/100000/*.root",
        "/opendata/eos/opendata/cms/Run2016G/DoubleEG/NANOAOD/UL2016_MiniAODv2_NanoAODv9-v1/1010000/*.root",
        "/opendata/eos/opendata/cms/Run2016G/DoubleEG/NANOAOD/UL2016_MiniAODv2_NanoAODv9-v1/250000/*.root"
    };

    TChain chain("Events");
    for (const auto& path : diretorios) {
        chain.Add(path.c_str());
    }

    TTreeReader reader(&chain);
    TTreeReaderArray<float> Electron_pt(reader, "Electron_pt");
    TTreeReaderArray<float> Electron_eta(reader, "Electron_eta");
    TTreeReaderArray<float> Electron_phi(reader, "Electron_phi");
    TTreeReaderArray<int> Electron_charge(reader, "Electron_charge");

    TFile outFile("MC.root", "RECREATE");
    TTree outTree("Electrons", "Árvore com elétrons selecionados");

    std::vector<float> pt, eta, phi;
    std::vector<int> charge;
    int nSelectedElectrons;

    outTree.Branch("Electron_pt", &pt);
    outTree.Branch("Electron_eta", &eta);
    outTree.Branch("Electron_phi", &phi);
    outTree.Branch("Electron_charge", &charge);
    outTree.Branch("N_electrons", &nSelectedElectrons);

    int eventos_analisados = 0;
    while (reader.Next()) {
        eventos_analisados++;
        if (eventos_analisados % 10000 == 0) {
            std::cout << "Eventos analisados: " << eventos_analisados << std::endl;
        }

        pt.clear();
        eta.clear();
        phi.clear();
        charge.clear();

        for (int i = 0; i < Electron_pt.GetSize(); ++i) {
            pt.push_back(Electron_pt[i]);
            eta.push_back(Electron_eta[i]);
            phi.push_back(Electron_phi[i]);
            charge.push_back(Electron_charge[i]);
        }

        nSelectedElectrons = pt.size();
        outTree.Fill();
    }

    // Salvando o arquivo
    outFile.Write();
    outFile.Close();

    std::cout << "Arquivo 'eletrons_selecionados.root' criado com sucesso!" << std::endl;
}
