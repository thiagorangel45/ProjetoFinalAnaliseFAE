#include <TMath.h>
#include <TLorentzVector.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLatex.h>
#include <iostream>
#include <vector>

void processData(const char* fileName, TH1F* dielectron_data_h, bool isMC)
{
    TFile *file = TFile::Open(fileName);
    if (!file || file->IsZombie()) {
        std::cerr << "Erro ao abrir o arquivo: " << fileName << std::endl;
        return;
    }

    TTree *tree = (TTree*)file->Get("Electrons");
    if (!tree) {
        std::cerr << "Erro: TTree 'Electrons' nao encontrado no arquivo: " << fileName << std::endl;
        file->Close();
        return;
    }

    std::vector<float> *Electron_pt = nullptr;
    std::vector<int> *Electron_charge = nullptr;
    std::vector<float> *Electron_eta = nullptr;
    std::vector<float> *Electron_phi = nullptr;

    tree->SetBranchAddress("Electron_pt", &Electron_pt);
    tree->SetBranchAddress("Electron_charge", &Electron_charge);
    tree->SetBranchAddress("Electron_eta", &Electron_eta);
    tree->SetBranchAddress("Electron_phi", &Electron_phi);

    int nevt = tree->GetEntries();
    std::cout << "Eventos para analisar: " << nevt << std::endl;

    TLorentzVector v1, v2, dielectron;

    for (int iev = 0; iev < nevt; iev++) {
        tree->GetEntry(iev);
        int nElectrons = Electron_pt->size();
        if (nElectrons < 4) continue;

        float ptCut = isMC ? 30.0 : 70.0;

        for (int i = 0; i < nElectrons; i++) {
            if ((*Electron_pt)[i] < ptCut || std::abs((*Electron_eta)[i]) > 2.4) continue;

            for (int j = 0; j < i; j++) {
                if ((*Electron_pt)[j] < ptCut || std::abs((*Electron_eta)[j]) > 2.4) continue;
                if ((*Electron_charge)[i] + (*Electron_charge)[j] != 0) continue;

                v1.SetPtEtaPhiM((*Electron_pt)[i], (*Electron_eta)[i], (*Electron_phi)[i], 0.000511);
                v2.SetPtEtaPhiM((*Electron_pt)[j], (*Electron_eta)[j], (*Electron_phi)[j], 0.000511);
                dielectron = v1 + v2;

                dielectron_data_h->Fill(dielectron.M());
            }
        }
    }

    file->Close();
}

void normalizeHistogram(TH1F* hist)
{
    Double_t integral = hist->Integral();
    if (integral > 0) {
        hist->Scale(1.0 / integral);
    } else {
        std::cerr << "Aviso: O histograma tem integral zero e nao pode ser normalizado!" << std::endl;
    }
}

void plotComparison(TH1F* dielectron_data_h, TH1F* dielectron_mc_h, TH1F* dielectron_mc_error_h)
{
    TCanvas *c1 = new TCanvas("c1", "Comparacao entre Dados e MC", 1000, 800);
    c1->SetTickx();
    c1->SetTicky();

    dielectron_data_h->SetLineColor(kBlack);
    dielectron_data_h->SetMarkerStyle(20);
    dielectron_data_h->SetMarkerSize(0.8);
    dielectron_data_h->SetStats(kFALSE);

    dielectron_mc_h->SetLineColor(kBlue);
    dielectron_mc_h->SetFillColorAlpha(kBlue, 0.5);
    dielectron_mc_h->SetStats(kFALSE);

    dielectron_mc_error_h->SetLineColor(kRed);
    dielectron_mc_error_h->SetFillColorAlpha(kRed, 0.5);
    dielectron_mc_error_h->SetStats(kFALSE);

    dielectron_data_h->GetXaxis()->SetTitle("M_{e^{+}e^{-}} [GeV]");
    dielectron_data_h->GetYaxis()->SetTitle("Eventos Normalizados / 1 GeV");

    dielectron_data_h->GetYaxis()->SetRangeUser(0, 0.15);
    dielectron_mc_h->GetYaxis()->SetRangeUser(0, 0.15);
    dielectron_mc_error_h->GetYaxis()->SetRangeUser(0, 0.15);

    dielectron_mc_error_h->Draw("E2");
    dielectron_mc_h->Draw("HIST SAME");
    dielectron_data_h->Draw("E1 SAME");

    TLegend *legend = new TLegend(0.65, 0.65, 0.9, 0.85);
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    legend->AddEntry(dielectron_data_h, "Dados", "lep");
    legend->AddEntry(dielectron_mc_h, "MC", "f");
    legend->AddEntry(dielectron_mc_error_h, "Incerteza estatistica", "f");
    legend->Draw();

    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.045);
    latex.SetTextAlign(13);
    latex.SetTextFont(42);
    latex.DrawLatex(0.10, 0.94, "CMS Open Data");
    latex.SetTextSize(0.035);
    latex.DrawLatex(0.15, 0.85, "pp 2016, #sqrt{s} = 13 TeV");
    latex.DrawLatex(0.15, 0.80, "p_{T}^{e} > 30 GeV, |#eta^{e}| < 2.4");

    c1->SaveAs("Data_vs_MC_Comparison_2.png");
}

void Data_vs_MC_mass_3()
{
    TH1F *dielectron_data_h = new TH1F("dielelectron_data_h", " ;M_{e^{+}e^{-}} [GeV];Eventos Normalizados / 1 GeV", 20, 80, 100);
    TH1F *dielectron_mc_h = new TH1F("dielelectron_mc_h", " ;M_{e^{+}e^{-}} [GeV];Eventos Normalizados / 1 GeV", 20, 80, 100);
    TH1F *dielectron_mc_error_h = new TH1F("dielelectron_mc_error_h", " ;M_{e^{+}e^{-}} [GeV];Eventos Normalizados / 1 GeV", 20, 80, 100);

    processData("/Users/thiagorangel/UERJ/Introducao_Analise_de_Dados_FAE/Projeto_Final_FAE/Data/MC.root", dielectron_mc_h, true);
    processData("/Users/thiagorangel/UERJ/Introducao_Analise_de_Dados_FAE/Projeto_Final_FAE/Data/Data.root", dielectron_data_h, false);
    processData("/Users/thiagorangel/UERJ/Introducao_Analise_de_Dados_FAE/Projeto_Final_FAE/Data/MC.root", dielectron_mc_error_h, true);

    normalizeHistogram(dielectron_data_h);
    normalizeHistogram(dielectron_mc_h);
    normalizeHistogram(dielectron_mc_error_h);

    plotComparison(dielectron_data_h, dielectron_mc_h, dielectron_mc_error_h);
}
