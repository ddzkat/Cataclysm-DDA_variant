#pragma once
#ifndef CATA_SRC_MATTACK_ACTORS_H
#define CATA_SRC_MATTACK_ACTORS_H

#include <climits>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "bodypart.h"
#include "damage.h"
#include "magic.h"
#include "mattack_common.h"
#include "mtype.h"
#include "translations.h"
#include "type_id.h"
#include "weighted_list.h"

class Creature;
class JsonObject;
class monster;

class leap_actor : public mattack_actor
{
    public:
        float max_range;
        // Jump has to be at least this tiles long
        float min_range;
        // Don't leap without a hostile target creature
        bool allow_no_target;
        int move_cost;
        // Range below which we don't consider jumping at all
        float min_consider_range;
        // Don't jump if distance to target is more than this
        float max_consider_range;

        leap_actor() = default;
        ~leap_actor() override = default;

        void load_internal( const JsonObject &obj, const std::string &src ) override;
        bool call( monster & ) const override;
        std::unique_ptr<mattack_actor> clone() const override;
};

class mon_spellcasting_actor : public mattack_actor
{
    public:
        // is the spell beneficial to target itself?
        bool self;
        spell spell_data;
        int move_cost;

        mon_spellcasting_actor() = default;
        ~mon_spellcasting_actor() override = default;

        void load_internal( const JsonObject &obj, const std::string &src ) override;
        bool call( monster & ) const override;
        std::unique_ptr<mattack_actor> clone() const override;
};

class melee_actor : public mattack_actor
{
    public:
        // Maximum damage from the attack
        damage_instance damage_max_instance = damage_instance::physical( 9, 0, 0, 0 );
        // Minimum multiplier on damage above (rolled per attack)
        float min_mul = 0.5f;
        // Maximum multiplier on damage above (also per attack)
        float max_mul = 1.0f;
        // Cost in moves (for attacker)
        int move_cost = 100;
        // If non-negative, the attack will use a different accuracy from mon's
        // regular melee attack.
        int accuracy = INT_MIN;

        /**
         * If empty, regular melee roll body part selection is used.
         * If non-empty, a body part is selected from the map to be targeted,
         * with a chance proportional to the value.
         */
        weighted_float_list<body_part> body_parts;

        /** Extra effects applied on damaging hit. */
        std::vector<mon_effect_data> effects;

        /** Message for missed attack against the player. */
        translation miss_msg_u;
        /** Message for 0 damage hit against the player. */
        translation no_dmg_msg_u;
        /** Message for damaging hit against the player. */
        translation hit_dmg_u;

        /** Message for missed attack against a non-player. */
        translation miss_msg_npc;
        /** Message for 0 damage hit against a non-player. */
        translation no_dmg_msg_npc;
        /** Message for damaging hit against a non-player. */
        translation hit_dmg_npc;

        melee_actor();
        ~melee_actor() override = default;

        virtual Creature *find_target( monster &z ) const;
        virtual void on_damage( monster &z, Creature &target, dealt_damage_instance &dealt ) const;

        void load_internal( const JsonObject &obj, const std::string &src ) override;
        bool call( monster & ) const override;
        std::unique_ptr<mattack_actor> clone() const override;
};

class bite_actor : public melee_actor
{
    public:
        // one_in( this - damage dealt ) chance of getting infected
        // i.e. the higher is this, the lower chance of infection
        int no_infection_chance;

        bite_actor();
        ~bite_actor() override = default;

        void on_damage( monster &z, Creature &target, dealt_damage_instance &dealt ) const override;

        void load_internal( const JsonObject &obj, const std::string &src ) override;
        std::unique_ptr<mattack_actor> clone() const override;
};

class gun_actor : public mattack_actor
{
    public:
        // Item type of the gun we're using
        itype_id gun_type;

        /** Specific ammo type to use or for gun default if unspecified */
        itype_id ammo_type = itype_id::NULL_ID();

        /*@{*/
        /** Balanced against player. If fake_skills unspecified defaults to GUN 4, WEAPON 8 */
        std::map<skill_id, int> fake_skills;
        int fake_str = 16;
        int fake_dex = 8;
        int fake_int = 8;
        int fake_per = 12;
        /*@}*/

        /** Specify weapon mode to use at different engagement distances */
        std::map<std::pair<int, int>, gun_mode_id> ranges;

        int max_ammo = INT_MAX; /** limited also by monster starting ammo */

        /** Description of the attack being run */
        std::string description;

        /** Message to display (if any) for failures to fire excluding lack of ammo */
        std::string failure_msg;

        /** Sound (if any) when either ammo depleted or max_ammo reached */
        std::string no_ammo_sound;

        /** Number of moves required for each attack */
        int move_cost = 150;

        /*@{*/
        /** Turrets may need to expend moves targeting before firing on certain targets */

        int targeting_cost = 100; /** Moves consumed before first attack can be made */

        bool require_targeting_player = true; /** By default always give player some warning */
        bool require_targeting_npc = false;
        bool require_targeting_monster = false;

        int targeting_timeout = 8; /** Default turns after which targeting is lost and needs repeating */
        int targeting_timeout_extend = 3; /** Increase timeout by this many turns after each shot */

        std::string targeting_sound;
        int targeting_volume = 6; /** If set to zero don't emit any targeting sounds */

        bool laser_lock = false; /** Does switching between targets incur further targeting penalty */
        /*@}*/

        /** If true then disable this attack completely if not brightly lit */
        bool require_sunlight = false;

        void shoot( monster &z, Creature &target, const gun_mode_id &mode ) const;

        gun_actor();
        ~gun_actor() override = default;

        void load_internal( const JsonObject &obj, const std::string &src ) override;
        bool call( monster & ) const override;
        std::unique_ptr<mattack_actor> clone() const override;
};

class wife_u_actor : public mattack_actor
{
    public:
        int move_cost;
        int corrupt_dice;
        int corrupt_dice_sides;
        int corrupt_turns;
        int mutate_chance;
        std::string mutate_category;
        int interval;
        int fake_dex = 8;

        wife_u_actor() = default;
        ~wife_u_actor() override = default;

        virtual player *find_target( monster &z ) const;

        void load_internal( const JsonObject &obj, const std::string &src ) override;
        bool call( monster & ) const override;
        std::unique_ptr<mattack_actor> clone() const override;
};

class summon_mon_actor : public mattack_actor
{
    public:
        int move_cost;
        float max_range;
        int delay = 150;
        std::string delay_var_str;
        std::string summon_msg;
        std::string trap_msg;
        int summoning_sickness;
        std::vector<mtype_id> mon_list;
        bool need_trap = false;
        trap_str_id tr_needed;

        summon_mon_actor() = default;
        ~summon_mon_actor() override = default;

        virtual player *find_target( monster &z ) const;

        void load_internal( const JsonObject &obj, const std::string &src ) override;
        bool call( monster & ) const override;
        std::unique_ptr<mattack_actor> clone() const override;
};

class expose_actor : public mattack_actor
{
    public:
        int move_cost;
        float max_range;
        bool only_sees;
        bool gain_corrupt;
        std::vector<mon_effect_data> effects;
        std::string snippet;

        expose_actor() = default;
        ~expose_actor() override = default;

        void load_internal( const JsonObject &obj, const std::string &src ) override;
        bool call( monster & ) const override;
        std::unique_ptr<mattack_actor> clone() const override;
};

class place_field_actor : public mattack_actor
{
    public:
        int move_cost;
        float max_range;
        std::string msg;
        bool self;
        std::vector<std::tuple<point, std::vector<field_type_str_id>>> fields;
        int intensity;
        int age;

        place_field_actor() = default;
        ~place_field_actor() override = default;

        player *find_target( monster &z ) const;

        void load_internal( const JsonObject &obj, const std::string &src ) override;
        bool call( monster & ) const override;
        std::unique_ptr<mattack_actor> clone() const override;
};

#endif // CATA_SRC_MATTACK_ACTORS_H
