/*
 * Six Sines A Sinnin'
 *
 * A mess, with audio rate modulation.
 *
 * Copyright 2024, Paul Walker and Various authors, as described in the github
 * transaction log.
 *
 * This source repo is released under the MIT license, but has
 * GPL3 dependencies, as such the combined work can also be
 * released under GPL3. You know the drill.
 */

#ifndef BACONPAUL_SIX_SINES_UI_MAIN_PANEL_H
#define BACONPAUL_SIX_SINES_UI_MAIN_PANEL_H

#include "sst/jucegui/components/Label.h"
#include "sst/jucegui/components/VUMeter.h"

#include "six-sines-editor.h"
#include "patch-data-bindings.h"

namespace baconpaul::six_sines::ui
{
struct MainPanel : jcmp::NamedPanel, HasEditor
{
    MainPanel(SixSinesEditor &);
    ~MainPanel();

    void resized() override;

    void beginEdit();

    std::unique_ptr<PatchContinuous> levData;
    std::unique_ptr<jcmp::Knob> lev;
    std::unique_ptr<jcmp::Label> levLabel;
    std::unique_ptr<jcmp::VUMeter> vuMeter;
};
} // namespace baconpaul::six_sines::ui
#endif // MAIN_PANEL_H
