#pragma once

#include <stdint.h>
#include "Settings.h"

void wheel_increment(Settings *settings);

class Selection;

class Selection {
    protected:
        Settings  *m_settings;
        Selection *m_prevSelection;
        Selection *m_nextSelection;

    public:
        Selection(Settings *settings, Selection *prevSelection, Selection *nextSelection);
        Selection *getPrevSelection();
        Selection *getNextSelection();
        virtual void setChange(int32_t change);
        virtual void draw(bool pressed);

};

class NullSelection : public Selection {
    public:
        NullSelection(Settings *settings, Selection *prevSelection, Selection *nextSelection);
        void draw(bool pressed) override;
};

class TemperatureSelection : public Selection {

    private:
        static const float TEMP_MIN;
        static const float TEMP_MAX;
        
    public:
        TemperatureSelection(Settings *settings, Selection *prevSelection, Selection *nextSelection);
        void draw(bool pressed) override;
        void setChange(int32_t change) override;
};


class VolumeSelection : public Selection {
    public:
        VolumeSelection(Settings *settings, Selection *prevSelection, Selection *nextSelection);
        void draw(bool pressed) override;
        void setChange(int32_t change) override;
};

class AboutSelection : public Selection {

    public:
        AboutSelection(Settings *settings, Selection *prevSelection, Selection *nextSelection);
        void draw(bool pressed) override;
        void setChange(int32_t change) override;
};


class Navigation {
    private:
        bool                    m_pressed;
        Selection              *m_currentSelection;

        NullSelection           m_nullSelection;
        TemperatureSelection    m_temperatureSelection;
        VolumeSelection         m_volumeSelection;
        AboutSelection          m_aboutSelection;
        
    public:
        Navigation(Settings *settings);
        void enterMenu();
        void exitMenu();
        void setChange(int32_t change);
        void draw();
};

extern Navigation nav;
